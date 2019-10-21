//
//  NetworkDetailView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 17/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct NetworkDetailView: View {
    
    var networkService: NetworkService
    
    var body: some View {
//        ScrollView{
//            VStack{
//                networkService.currentNetwork.flatMap{
//                     CurrentNetworkView(currentNetwork: $0)
//                }
//
//                networkService.availableNetworks.flatMap{
//                    AvailableNetworksView($0, networkService: networkService)
//                }
//
//            }.padding()
//        }.navigationBarTitle(Text("Network Settings"), displayMode:.large)
        Text("Temp")
    }
}

struct NetworkDetailView_Previews: PreviewProvider {
    static var previews: some View {
        let networkService = NetworkService()
//        networkService.currentNetwork = CurrentNetwork(status: 13, connected: false, ssid: "Some Net", channel: 1, ip: nil, rssi: -20)
//        networkService.availableNetworks = [
//                    AvailableNetwork(ssid: "Broccoli", enctype: .open, rssi: -20, channel: 4, bssid:"SOMETHING"),
//            AvailableNetwork(ssid: "My Wifi is Nice", enctype: .open, rssi: -20, channel: 4, bssid:"mywifi")
//        ]
        return NetworkDetailView(networkService: networkService)
//        return NetworkDetailView()
    }
}
