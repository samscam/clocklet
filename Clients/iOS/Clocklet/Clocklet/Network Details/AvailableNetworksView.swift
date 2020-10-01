//
//  AvailableNetworksView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 17/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import CombineBluetooth

struct AvailableNetworksView: View {
    @EnvironmentObject var networkService: NetworkService
    
    @State var showPopover: Bool = false
    
    var body: some View {
        ConfigItemView(icon: Image(systemName:"antenna.radiowaves.left.and.right"), title: "Available networks") {
            VStack(alignment: .leading, spacing: 5){
                

                    ForEach(self.networkService.scannedNetworks)
                        { network in

                        
                            NavigationLink(destination:EnterPasswordView(network: network, networkService: self.networkService)){
                                AvailableNetworkView(network: network)
                            }.buttonStyle(PlainButtonStyle())
                        }
                Divider()
                
                NavigationLink(destination: EnterPasswordView( networkService: self.networkService)){
                    Text("Other network...").italic().padding(EdgeInsets(top: 10, leading: 0, bottom: 10, trailing: 0))
                }.buttonStyle(PlainButtonStyle())
                

            }

        }

    }
}

extension Network{
    var wifiStrengthBars: Image {
        if (self.rssi >= -60) {
            return Image(uiImage: UIImage(systemName:"wifi")!.withRenderingMode(.alwaysTemplate))
        } else if (self.rssi >= -80) {
            return Image(uiImage: UIImage(named:"wifi.2bars")!.withRenderingMode(.alwaysTemplate))
        } else {
            return Image(uiImage: UIImage(named:"wifi.1bar")!.withRenderingMode(.alwaysTemplate))
        }
    }
}

struct NetworkStatusIconView: View{
    let network:  Network
    var body: some View{
        ZStack(alignment:.bottom){
            Image(uiImage: UIImage(systemName:"wifi")!.withRenderingMode(.alwaysTemplate)).opacity(0.2)
            network.wifiStrengthBars.foregroundColor(.green)
        }
    }
}

struct AvailableNetworkView: View {
    var network: AvailableNetwork
    var body: some View {
        HStack(alignment: .center){
            
            
            NetworkStatusIconView(network: network)
            
            Text(network.ssid).font(.headline).bold().lineLimit(2)
            
            Spacer()
            
            if network.enctype == .open {
                Image(systemName:"arrow.left.and.right.circle").resizable().scaledToFit().frame(width: 30, height: 40, alignment: .center)
            } else {
                Image(systemName:"lock.circle").resizable().scaledToFit().frame(width: 30, height: 40, alignment: .center)
            }
            


        }.padding(EdgeInsets(top: 5, leading: 0, bottom: 5, trailing: 0))
    }
        
}

struct AvailableNetworkView_Previews: PreviewProvider {
    static var previews: some View {
        let network = AvailableNetwork(ssid: "Broccoli", enctype: .open, rssi: -80, channel: 4, bssid:"SOMETHING")
        return Group {
            AvailableNetworkView(network: network).previewLayout(.sizeThatFits)
            AvailableNetworkView(network: network).previewLayout(.sizeThatFits)
        }
    }
}
