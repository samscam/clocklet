//
//  ClockConfigurator.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 08/12/2022.
//  Copyright © 2022 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct ClockConfigurator: View {
    @EnvironmentObject var clock: Clock
    
    var body: some View {
        Group{
            Text("Set up your clocklet...").font(.largeTitle)
            
            if let networkService = clock.networkService,
               networkService.isConfigured == .notConfigured {
                AvailableNetworksView().environmentObject(networkService)
                    .onAppear{
                        networkService.$availableNetworks.shouldNotify = true
                        
                    }
                    .onDisappear{
                        networkService.$availableNetworks.shouldNotify = false
                    }
            } else {
                if let locationService = clock.locationService {
                    if (locationService.isConfigured == .notConfigured){
                        Button("Set to current location"){
                            locationService.setCurrentLocation()
                        }.buttonStyle(RoundyButtonStyle())
                    }
                }
            }
        }
            .onAppear{
                
                clock.networkService?.$currentNetwork.shouldNotify = true
            }.onDisappear{
                clock.networkService?.$currentNetwork.shouldNotify = false
            }
    }
}

struct ClockConfigurator_Previews: PreviewProvider {
    static var previews: some View {
        ClockConfigurator().environmentObject(Clock("What?"))
    }
}
