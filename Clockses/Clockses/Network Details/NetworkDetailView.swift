//
//  NetworkDetailView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 17/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import Combine
import CombineBluetooth

struct NetworkDetailView: View {
    
    @EnvironmentObject var networkService: NetworkService
    
    var body: some View {
        ScrollView{
            VStack{
                
                NetworkSummaryView()
//
//                networkService.availableNetworks.map{
//                    AvailableNetworksView()
//                }
//                self.viewModel.availableNetworks.map{
//                    AvailableNetworksView($0,networkService: self.viewModel.networkService)
//                }
//                
            }.padding()
        }.navigationBarTitle(Text("Network Settings"), displayMode:.large)
        
    }
}

struct NetworkDetailView_Previews: PreviewProvider {
    static var previews: some View {
        let networkService = NetworkService()
        networkService.currentNetwork = CurrentNetwork(status: .connectionFailed, connected: false, ssid: "Some Net", channel: 1, ip: nil, rssi: -20)
        networkService.availableNetworks = [
                    AvailableNetwork(ssid: "Broccoli", enctype: .open, rssi: -20, channel: 4, bssid:"SOMETHING"),
            AvailableNetwork(ssid: "My Wifi is Nice", enctype: .open, rssi: -20, channel: 4, bssid:"mywifi")
        ]
        return NetworkDetailView().environmentObject(networkService)
        
    }
}
