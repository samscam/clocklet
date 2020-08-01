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
        ConfigItemView(icon: Image(systemName:"location"), title: "Location") {
            self.locationService.currentLocation.map{ currentLocation in
                VStack(alignment:.leading){
                        
                    if currentLocation.configured {
                        self.locationService.placemark.map{ placemark in
                            VStack(alignment:.leading){
                                HStack{
                                    Text("Place:").foregroundColor(.secondary)
                                    Text(placemark.locality ?? "Unknown Place").bold().foregroundColor(.primary)
                                }
                                
                                HStack{
                                    Text("Time zone:").foregroundColor(.secondary)
                                    Text(placemark.timeZone?.identifier ?? "Unknown time zone").bold().foregroundColor(.primary)
                                }
                            }
                        }
                    } else {
                        
                        Button("Set to current location"){
                                self.locationService.setCurrentLocation()
                            }
                            .buttonStyle(RoundyButtonStyle())
                        
                    }
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

/**
 
 
 locationService.placemark.map{ placemark in
     HStack{
         Text("City:").bold()
         Text(placemark.locality ?? "Unknown Place")
     }
     
     HStack{
         Text("Time zone:").bold()
         Text(placemark.timeZone?.identifier ?? "Unknown time zone")
     }
 }
 
 
 Button(action: {
     locationService.setCurrentLocation()
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
 */
