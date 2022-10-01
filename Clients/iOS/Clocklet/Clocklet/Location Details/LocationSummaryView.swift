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
    @Binding var showLocationDetails: Bool
    
    @EnvironmentObject var locationService: LocationService
    
    var body: some View {
        ConfigItemView(
            icon: Image(systemName:"location"),
            title:
                self.locationService.isConfigured == .configured ? self.locationService.currentLocation?.placeName  ?? "Location" : "Location",
            disclosure: true) {
                if let currentLocation = locationService.currentLocation {
                    
                    if currentLocation.configured {
                        EmptyView()
                    } else {
                       
                        VStack(alignment:.leading){
                            Button("Set to current location"){
                                self.locationService.setCurrentLocation()
                            }
                            .buttonStyle(RoundyButtonStyle())
                            
                            Button( action: {
                                showLocationDetails = true
                            }, label: {
                                HStack{
                                    Text( "Set to somewhere else" )
                                    Image(systemName: "chevron.right").resizable().aspectRatio(contentMode: .fit).frame(width: 20, height: 20)
                                }
                            })
                            .buttonStyle(RoundyButtonStyle()).accentColor(.green)
                        }
                        
                    }
                    
                    
                }
            }
    }
}
