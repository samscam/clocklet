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

//class LocationSummaryViewModel: ObservableObject{
//
//    @Published var isResolving: Bool = true
//    @Published var title: String = "Resolving..."
//    @Published var coordinates: String? = nil
//    @Published var coord2d: CLLocationCoordinate2D? = nil
//
//    private var locationService: LocationService?
//
//    private var locationProxy = LocationProxy()
//
//    var bag: [AnyCancellable] = []
//
//    init(_ locationService: LocationService?){
//        self.locationService = locationService
//
//        guard let locationService = locationService else {
//            return
//        }
//
//        locationService.$currentLocation.compactMap{
//            $0?.description
//        }
//        .catch{ error in
//            return Just("Error: \(error.localizedDescription)")
//        }
//        .assign(to: \.coordinates, on: self)
//            .store(in: &bag)
//
//        locationService
//            .$currentLocation
//            .compactMap{$0?.location}
//            .flatMap{ location in
//                GeocoderProxy().futureReversePublisher(location)
//            }
//            .catch { (error) in
//                Just("Error: \(error.localizedDescription)")
//            }
//            .assign(to: \.title, on: self)
//            .store(in: &bag)
//
//
//        locationService
//            .$currentLocation
//            .compactMap{$0?.location.coordinate}
//            .catch{ error in
//                Just(nil)
//            }
//            .assign(to: \.coord2d, on: self)
//            .store(in: &bag)
//
//
//    }
//
//
//    func setCurrentLocation(){
//
//        guard let locationService = locationService else {
//            return
//        }
//
//        locationProxy.locationPublisher.sink(receiveCompletion: { (completion) in
//
//        }) { (location) in
//            locationService.currentLocation = CurrentLocation(lat: location.coordinate.latitude, lng: location.coordinate.longitude)
//        }.store(in: &bag)
//    }
//
//}

struct LocationSummaryView: View {
    
    @EnvironmentObject var locationService: LocationService
    
    
    var body: some View {
        ConfigItemView(icon: Image(systemName:"location"), title: "Location") {
                VStack(alignment:.leading){
                    
                    self.locationService.placemark.map{ placemark in
                        Text(placemark.locality ?? "Unknown Place")
                    }
                    
                    self.locationService.placemark.map{ placemark in
                        Text(placemark.timeZone?.identifier ?? "Unknown time zone")
                    }
                    
                    self.locationService.currentLocation.map{
                        
                        MapView(coordinate: $0.location.coordinate)
                            .clipShape(RoundedRectangle(cornerRadius: 10))
                            .frame(height: 150)
                    }
                    
                    Button(action: {
                        self.locationService.setCurrentLocation()
                    }) {
                        HStack{
                            Spacer()
                            Text("Set to current location")
                            .font(.headline)
                            .foregroundColor(.white)
                            .padding()
                            Spacer()
                        }
                        
                        
                        
                    }
                    .background(Color.gray)
                    .clipShape(RoundedRectangle(cornerRadius:10))
                    
                }
            }
        
    }
}

//struct LocationSummaryView_Previews: PreviewProvider {
//    static var locationService = LocationService()
//    static let viewModel = LocationSummaryViewModel(locationService)
//
//    static var previews: some View {
//        locationService.currentLocation = CurrentLocation(lat: 0, lng: 0)
//        DispatchQueue.main.asyncAfter(deadline: .now() + 5) {
//            locationService.currentLocation = CurrentLocation(lat: 53.431808, lng: -2.218080)
//        }
//        return LocationSummaryView()
//    }
//}
//
