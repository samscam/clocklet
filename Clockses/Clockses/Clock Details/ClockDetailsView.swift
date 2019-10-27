//
//  ClockDetailsView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 10/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import Network
import CombineBluetooth

extension  ConnectionState{
    var iconSystemName: String {
        switch self {
            case .connected: return "bolt.fill"
            case .connecting: return "bolt"
            case .disconnected: return "bolt.slash.fill"
        }
    }
    
}
extension ConnectionState: CustomStringConvertible {

    public var description: String {
        switch self {
            case .connected: return "Connected"
            case .connecting: return "Connecting"
            case .disconnected: return "Disconnected"
        }
    }
}

struct ClockDetailsView: View {
    
    @ObservedObject var viewModel: ClockDetailsViewModel
    
    var showCurrentNetwork: Bool {
        return viewModel.clock.networkService.currentNetwork != nil
    }
    
    var body: some View {
        ScrollView{
            VStack(){
                Image(viewModel.clock.caseColour.imageName).resizable().aspectRatio(contentMode: .fit)

                ConfigItemView(iconSystemName: viewModel.clock.state.iconSystemName,
                               title: viewModel.clock.state.description) {
                    EmptyView()
                }.transition(.opacity)

            
                NavigationLink(destination:NetworkDetailView(networkService:viewModel.clock.networkService)){
                    viewModel.clock.networkService.currentNetwork.flatMap{
                         CurrentNetworkView(currentNetwork: $0)
                    }
                }
                
            viewModel.locationServiceViewModel.map{LocationServiceView($0)}

            }
            .padding()
            .animation(.default)
            .onAppear {
                self.viewModel.clock.connect()
            }.onDisappear {
//                self.viewModel.clock.disconnect()
            }

        }.navigationBarTitle(Text(viewModel.clock.name), displayMode:.large)
    }
}



struct ClockDetailsView_Previews: PreviewProvider {
    static var previews: some View {
        Text("FAIL CITY")
//        ClockDetailsView(clock: Clock(uuid: UUID(), name: "Some Clock"))
//        let clock = ClockModel(id: UUID(), serial: 5, name: "Clocklet #291", caseColour: .wood)
//
//
//        let currentNetwork = CurrentNetwork(status: 4, connected: true, ssid: "Broccoli", channel: 5, ip: IPv4Address("129.12.41.5"), rssi: -2)
//        let networkService = NetworkService()
//        networkService.currentNetwork = currentNetwork
//        networkService.availableNetworks = [
//        AvailableNetwork(ssid: "Broccoli", enctype: .open, rssi: -20, channel: 4, bssid:"SOMETHING"),
//        AvailableNetwork(ssid: "My Wifi is Nice", enctype: .open, rssi: -20, channel: 4, bssid:"mywifi")
//        ]
//        let connection = ClockConnection(clock: clock, networkService: networkService)
//        return ClockDetailsView(clockConnection: connection)
    }
}

