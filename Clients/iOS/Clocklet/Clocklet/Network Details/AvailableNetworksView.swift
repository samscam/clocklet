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
        if #available(iOS 16, *) {
            
            // roughly scale rssi to the SFSymbols image variable boundaries
            let strength: Double =  min(max((Double(self.rssi) + 90) * 0.03,0.0),1.0)
            return Image(systemName: "wifi", variableValue: strength)
            
         } else {
            if (self.rssi >= -60) {
                return Image(uiImage: UIImage(systemName:"wifi")!.withRenderingMode(.alwaysTemplate))
            } else if (self.rssi >= -80) {
                return Image(uiImage: UIImage(named:"wifi.2bars")!.withRenderingMode(.alwaysTemplate))
            } else {
                return Image(uiImage: UIImage(named:"wifi.1bar")!.withRenderingMode(.alwaysTemplate))
            }
        }
    }
}

struct NetworkStatusIconView: View{
    let network:  Network
    var body: some View{
        if #available(iOS 16, *) {
            network.wifiStrengthBars.foregroundColor(.green)
        } else {
            ZStack(alignment:.bottom){
                Image(uiImage: UIImage(systemName:"wifi")!.withRenderingMode(.alwaysTemplate)).opacity(0.2)
                network.wifiStrengthBars.foregroundColor(.green)
            }
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
            
            encryptionIcon(enctype: network.enctype).resizable().scaledToFit().frame(width: 30, height: 40, alignment: .center)
            
        }.padding(EdgeInsets(top: 5, leading: 0, bottom: 5, trailing: 0))
            .contentShape(Rectangle())
    }
    
    func encryptionIcon(enctype: AuthMode) -> Image{
        let image: Image
        switch enctype {
        case .open:
            image = Image(systemName:"arrow.left.and.right.circle")
        case .unknown:
            image = Image(systemName:"questionmark.circle")
        default:
            image = Image(systemName:"lock.circle")
        }
        return image
    }
}

struct AvailableNetworksView_Previews: PreviewProvider {
    
    static let popover = false
    static var fakeNetworkService: NetworkService {
        let networkService = NetworkService()
        
        let network1 = AvailableNetwork(ssid: "Whyfly? Taketrain!", enctype: .open, rssi: -93, channel: 4, bssid:"whyfly")
        let network2 = AvailableNetwork(ssid: "Pretty wi for a fi guy", enctype: .wep, rssi: -85, channel: 4, bssid:"prettywi")
        let network3 = AvailableNetwork(ssid: "Broccoli", enctype: .unknown, rssi: -73, channel: 4, bssid:"brocc")
        
        let network5 = AvailableNetwork(ssid: "Pretty wi for a fi guy", enctype: .wep, rssi: -63, channel: 4, bssid:"wowoow")
        networkService.scannedNetworks = [network1,network2,network3,network5]
        
        return networkService
    }
    
    static var previews: some View {
        VStack{
            AvailableNetworksView().environmentObject(fakeNetworkService)
        }
    }
}
