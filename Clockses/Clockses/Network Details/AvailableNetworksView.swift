//
//  AvailableNetworksView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 17/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct AvailableNetworksView: View {
    var availableNetworks: [AvailableNetwork]
    
    let networkService: NetworkService?
    
    init(_ availableNetworks: [AvailableNetwork], networkService: NetworkService? = nil){
        self.availableNetworks = availableNetworks
        self.networkService = networkService
    }
    
    @State var showPopover: Bool = false
    @State var selectedNetwork: AvailableNetwork? = nil
    
    var body: some View {
        ConfigItemView(icon: Image(systemName:"antenna.radiowaves.left.and.right"), title: "Available networks") {
            VStack{
                ForEach(self.availableNetworks){ network in
                    
                    Button(action: {
                        self.selectedNetwork = network
                    }) {
                        AvailableNetworkView(network: network)
                    }
                }
            }
            .popover(item: self.$selectedNetwork) { (network) -> EnterPasswordView in
                EnterPasswordView(network: network, networkService: self.networkService)
            }
        }

    }
}

struct AvailableNetworkView: View {
    var network: AvailableNetwork
    var body: some View {
        HStack(alignment: .top){
            Image(systemName:"wifi").resizable().scaledToFit().frame(width: 20, height: 20, alignment: .center).padding(.trailing, 5)
            VStack(alignment: .leading) {

                Text(network.ssid).font(.headline).bold().lineLimit(2)
                
                HStack {
                    Text("Channel:")
                    Text(String(network.channel)).bold()
                    Spacer()
                    Text("Signal:")
                    Text(String(network.rssi)).bold()
                    Spacer()
                    Text("Security:")
                    Text(String(network.enctype.description)).bold()

                    }.font(.caption)
            }
            
        }.padding(EdgeInsets(top: 5, leading: 0, bottom: 5, trailing: 0))
    }
}

struct AvailableNetworkView_Previews: PreviewProvider {
    static var previews: some View {
        let network = AvailableNetwork(ssid: "Broccoli", enctype: .open, rssi: -20, channel: 4, bssid:"SOMETHING")
        return AvailableNetworkView(network: network).previewLayout(.sizeThatFits)
    }
}
