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

class LocationServiceViewModel: ObservableObject{
    @Published var isResolving: Bool = true
    @Published var title: String = "Resolving..."
    @Published var coordinates: String? = nil

    private var locationService: LocationService?
    
    var cancellables: [AnyCancellable] = []
    
    init(_ locationService: LocationService?){
        self.locationService = locationService
        if let locationService = locationService {
            
            let can = locationService.$currentLocation.publisher.compactMap{
                $0??.description
            }.assign(to: \.coordinates, on: self)
            cancellables.append(can)
            
            let composed = locationService
                .$currentLocation
                .publisher
                .compactMap{$0??.location}
                .flatMap{ GeocoderProxy().futureReversePublisher($0).catch { (error) -> AnyPublisher<String,Never> in
                    return Just("Oh dear").eraseToAnyPublisher()
                    }
            }.assign(to: \.title, on: self)
            
            cancellables.append(composed)
        }
    }

}

struct LocationServiceView: View {
    @ObservedObject var viewModel: LocationServiceViewModel
    
    init(_ viewModel: LocationServiceViewModel){
        self.viewModel = viewModel
    }
    
    var body: some View {
        ConfigItemView(iconSystemName: "location", title: viewModel.title) {
            VStack{
                self.viewModel.coordinates.map{Text($0)}
            }
        }
       
    }
}

struct LocationServiceView_Previews: PreviewProvider {
    static var locationService = LocationService()
    static let viewModel = LocationServiceViewModel(locationService)
    
    static var previews: some View {
        locationService.currentLocation = CurrentLocation(lat: 0, lng: 0)
        DispatchQueue.main.asyncAfter(deadline: .now() + 5) {
            locationService.currentLocation = CurrentLocation(lat: 53.431808, lng: -2.218080)
        }
        return LocationServiceView(viewModel)
    }
}
