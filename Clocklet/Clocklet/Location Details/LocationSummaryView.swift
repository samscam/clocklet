//
//  LocationServiceView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 17/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import Combine
import CoreLocation


struct LocationSummaryView: View {
    
    @EnvironmentObject var locationService: LocationService
    
    var body: some View {
        ConfigItemView(icon: Image(systemName:"location"), title:
                        self.locationService.isConfigured == .configured ? self.locationService.currentLocation?.placeName  ?? "Location" : "Location") {
            self.locationService.currentLocation.map{ currentLocation -> AnyView in
                if currentLocation.configured {
                    return AnyView(EmptyView())
                } else {
                    return AnyView(
                        HStack(alignment:.center){
                        Button("Set to current location"){
                                self.locationService.setCurrentLocation()
                            }
                            .buttonStyle(RoundyButtonStyle())
                            Button("More >"){
                                    self.locationService.setCurrentLocation()
                                }
                            .buttonStyle(RoundyButtonStyle()).accentColor(.green)
                    })
                    
                }
            

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
