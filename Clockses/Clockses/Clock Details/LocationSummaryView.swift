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
