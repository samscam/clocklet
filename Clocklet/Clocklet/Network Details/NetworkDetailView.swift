//
//  NetworkDetailView.swift
//  Clocklet
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
                
                NetworkHeaderView().environmentObject(NetworkSummaryViewModel(networkService))
                networkService.currentNetwork.map{ currentNetwork in
                    ConfigItemView(icon: Image(systemName:"waveform.path"), iconColor: nil, title: "Details") {
                        HStack{
                            VStack(alignment:.leading){
                                Text("IP Address:")
                                Text("Channel:")
                            }
                            VStack(alignment:.leading){
                                Text(currentNetwork.ip?.debugDescription ?? "No ip address")
                                Text("\(currentNetwork.channel)")
                            }
                        }
                    }
                }
                
                networkService.availableNetworks.map{ _ in
                    AvailableNetworksView().environmentObject(self.networkService.$availableNetworks)
                }
            }.padding()
        }.navigationBarTitle(Text("Network Settings"), displayMode:.large)
        
    }
}

struct NetworkDetailView_Previews: PreviewProvider {
    static let networkService: NetworkService = {  
        let networkService = NetworkService()
        networkService.currentNetwork = CurrentNetwork(status: .connectionFailed, connected: false, configured: true, ssid: "Some Net", channel: 1, ip: nil, rssi: -20)
        networkService.availableNetworks = [
                    AvailableNetwork(ssid: "Broccoli", enctype: .open, rssi: -20, channel: 4, bssid:"SOMETHING"),
            AvailableNetwork(ssid: "My Wifi is Nice", enctype: .open, rssi: -20, channel: 4, bssid:"mywifi")
        ]
        return networkService
        
    }()
    
    static var previews: some View {

        return NetworkDetailView().environmentObject(networkService)
        
    }
}
