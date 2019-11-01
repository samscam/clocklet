//
//  LocationServiceView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 17/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import Combine
import CoreLocation

class LocationSummaryViewModel: ObservableObject{
    @Published var isResolving: Bool = true
    @Published var title: String = "Resolving..."
    @Published var coordinates: String? = nil
    @Published var coord2d: CLLocationCoordinate2D? = nil

    private var locationService: LocationService?
    
    var bag: [AnyCancellable] = []
    
    init(_ locationService: LocationService?){
        self.locationService = locationService
        
        guard let locationService = locationService else {
            return
        }
            
        locationService.$currentLocation.publisher.compactMap{
            $0??.description
        }.assign(to: \.coordinates, on: self)
        .store(in: &bag)
        
        locationService
            .$currentLocation
            .publisher
            .compactMap{$0??.location}
            .flatMap{ GeocoderProxy().futureReversePublisher($0).catch { (error) -> AnyPublisher<String,Never> in
                return Just("Oh dear").eraseToAnyPublisher()
                }
        }.assign(to: \.title, on: self)
            .store(in: &bag)
        
        
        locationService
            .$currentLocation
            .publisher
            .compactMap{$0??.location.coordinate}
            .assign(to: \.coord2d, on: self)
            .store(in: &bag)
       
        
    }

}

struct LocationSummaryView: View {
    @ObservedObject var viewModel: LocationSummaryViewModel
    
    init(_ viewModel: LocationSummaryViewModel){
        self.viewModel = viewModel
    }
    
    var body: some View {
        ConfigItemView(icon: Image(systemName:"location"), title: viewModel.title) {
            VStack(alignment:.leading){
                
                self.viewModel.coord2d.map{
                    MapView(coordinate: $0)
                    .clipShape(RoundedRectangle(cornerRadius: 10))
                    .frame(height: 150)
                }
                self.viewModel.coordinates.map{Text($0).font(.caption)}
            }
        }
       
    }
}

struct LocationSummaryView_Previews: PreviewProvider {
    static var locationService = LocationService()
    static let viewModel = LocationSummaryViewModel(locationService)
    
    static var previews: some View {
        locationService.currentLocation = CurrentLocation(lat: 0, lng: 0)
        DispatchQueue.main.asyncAfter(deadline: .now() + 5) {
            locationService.currentLocation = CurrentLocation(lat: 53.431808, lng: -2.218080)
        }
        return LocationSummaryView(viewModel)
    }
}

