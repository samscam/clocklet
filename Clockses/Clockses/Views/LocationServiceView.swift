//
//  LocationServiceView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 17/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import Combine

struct LocationServiceView: View {
    @ObservedObject var locationService: LocationService
    
    var body: some View {
        ConfigItemView(iconSystemName: "location", title: "Withington, Manchester, UK") {
            Text("\(self.locationService.currentLocation?.description ?? "no location")")
            
        }
       
    }
}

struct LocationServiceView_Previews: PreviewProvider {
    static let locationService = LocationService()
    
    static var previews: some View {
        LocationServiceView(locationService: locationService)
    }
}
