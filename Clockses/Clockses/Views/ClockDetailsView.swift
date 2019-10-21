//
//  ClockDetailsView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 10/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import Network

struct ClockDetailsView: View {
    
    @ObservedObject var clockConnection: ClockConnection
    
    var clock: Clock {
        return clockConnection.clock
    }
    
//    var networkService: NetworkService? {
//        return clockConnection.networkService
//    }
    
    var showCurrentNetwork: Bool {
        return self.clockConnection.networkService?.currentNetwork != nil
    }
    
    var body: some View {
        ScrollView{
           VStack(alignment: .leading){
                Image(clock.caseColour.imageName).resizable().aspectRatio(contentMode: .fit)
            
                ConfigItemView(iconSystemName: clockConnection.state.iconSystemName,
                               title: clockConnection.state.description) {
                    EmptyView()
                }.transition(.opacity)
            
            clockConnection.networkService.map{ networkService in
                NavigationLink(destination:NetworkDetailView(networkService:networkService)){
                                networkService.currentNetwork.flatMap{
                         CurrentNetworkView(currentNetwork: $0)
                    }
                }
            }
            
//                clockConnection.networkService.flatMap{$0.currentNetwork}.map{
//                    NavigationLink(destination: AvailableNetworksView(networkService)) {
//                         CurrentNetworkView(currentNetwork: $0).transition(.opacity)
//                    }
//                }
//

                LocationServiceView()
            
            }
            .padding()
           .animation(.default)
            .onAppear {
                self.clockConnection.connect()
            }.onDisappear {
//                self.clockConnection.disconnect()
            }
        
        }.navigationBarTitle(Text(clockConnection.clock.name), displayMode:.large)
    }
}



struct ClockDetailsView_Previews: PreviewProvider {
    static var previews: some View {
        let clock = Clock(id: UUID(), serial: 5, name: "Clocklet #291", caseColour: .wood)
        
        
        let currentNetwork = CurrentNetwork(status: 4, connected: true, ssid: "Broccoli", channel: 5, ip: IPv4Address("129.12.41.5"), rssi: -2)
        let networkService = NetworkService()
        networkService.currentNetwork = currentNetwork
        networkService.availableNetworks = [
        AvailableNetwork(ssid: "Broccoli", enctype: .open, rssi: -20, channel: 4, bssid:"SOMETHING"),
        AvailableNetwork(ssid: "My Wifi is Nice", enctype: .open, rssi: -20, channel: 4, bssid:"mywifi")
        ]
        let connection = ClockConnection(clock: clock, networkService: networkService)
        return ClockDetailsView(clockConnection: connection)
    }
}

