//
//  NetworkStatusView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 10/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import Network




struct Prop: Identifiable {
    let title:String
    let value:String
    
    init(_ title: String, _ value: String){
        self.title = title
        self.value = value
    }
    
    var id: String {
        return title
    }
}

struct PropertiesView: View{
    
    var properties: [Prop]
    
    var body: some View {
        HStack(spacing:5){
            VStack(alignment: .leading, spacing: 5){
                ForEach(properties){ prop in
                    Text(prop.title + ":")
                }
            }
            VStack(alignment: .leading, spacing: 5){
                ForEach(properties){ prop in
                    Text(prop.value).bold()
                }
            }
        }.font(.caption)
    }
}
struct PropertiesView_Previews: PreviewProvider {
    static var previews: some View {
        
        PropertiesView(properties:[Prop("IP","192.168.1.5"),
                                   Prop("Signal","-20"),
                                   Prop("Security","Closed")])
            
            .previewLayout(.sizeThatFits)
    }
}

extension CurrentNetwork {
    var title: String {
        if self.connected && self.ssid != "" {
            return self.ssid
        }
        return "Not connected"
    }
    
    var icon: String {
        switch connected {
        case true: return "wifi"
        case false: return "wifi.slash"
        }
    }
}
struct CurrentNetworkView: View {
    
    @State var currentNetwork: CurrentNetwork
    
    
    var body: some View {
        
        ConfigItemView(iconSystemName: currentNetwork.icon, title: currentNetwork.title) {
            return PropertiesView(properties: [Prop("IP address",self.currentNetwork.ip?.debugDescription ?? "No IP address")])
        }
    }
}



struct CurrentNetworkView_Previews: PreviewProvider {
    static var networkService: NetworkService {
        let networkService = NetworkService()
        networkService.currentNetwork = CurrentNetwork(status: 3, connected: true, ssid: "fish", channel: 5, ip: IPv4Address("192.167.234.12"), rssi: -20)
        
        networkService.availableNetworks = [
            AvailableNetwork(ssid: "Best Wifi", enctype: .wep, rssi: -30, channel: 5, bssid:"MAC"),
            AvailableNetwork(ssid: "My WiFi is better than your WiFi", enctype: .wpa2psk, rssi: -10, channel: 1, bssid:"MOOK")
        ]
        return networkService
    }
    
    static var currentNetwork: CurrentNetwork {
        return CurrentNetwork(status: 3, connected: true, ssid: "fish", channel: 5, ip: IPv4Address("192.167.234.12"), rssi: -20)
    }
    
    static var previews: some View {
        CurrentNetworkView(currentNetwork: currentNetwork)
            .previewLayout(.sizeThatFits)
        
    }
}



struct EnterPasswordView: View {
    let network: AvailableNetwork
    let networkService: NetworkService?
    
    @State private var password: String = ""
    
    var body: some View {
        VStack{
            
            Text(network.ssid).font(.title).bold().lineLimit(2)

            SecureField("Password", text: $password)
                .font(.title)
                .padding()
                .background(Color.gray)
                .cornerRadius(10)
                .overlay(RoundedRectangle(cornerRadius: 10, style: .circular).stroke(Color.black, lineWidth: 3)
                    
            )
            HStack{
                Button(action: {}) {
                    Text("Cancel")
                }.accentColor(.red)
                Spacer()
                Button(action:{
                    try? self.networkService?.joinNetwork(self.network, psk: self.password)
                    
                }){
                    Text("Join").bold()
                }
                .accentColor(.green)
            }
            
        }.padding()
    }
}

struct EnterPasswordView_Previews: PreviewProvider {
    static var previews: some View {
        EnterPasswordView(network: AvailableNetwork(ssid: "Testnet", enctype: .wpa2enterprise, rssi: -15, channel: 2, bssid: "bums"), networkService: nil)
            .previewLayout(.sizeThatFits)
        
    }
}
