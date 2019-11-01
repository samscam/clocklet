//
//  NetworkStatusView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 10/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import Network
import Combine
import CombineBluetooth

class NetworkSummaryViewModel: ObservableObject{
    @Published var title: String = ""
    @Published var errorMessage: String? = nil
    @Published var icon: Image = Image(systemName:"questionmark.circle")
    @Published var color: Color = .black
    
    private var bag = [AnyCancellable]()
    
    private var networkService: NetworkService
    
    init(_ networkService: NetworkService){
        self.networkService = networkService
        
        networkService.$currentNetwork.publisher.sink { [weak self] (currentNetwork) in
            guard let self = self else {
                return
            }
            if let w1 = currentNetwork,
                let currentNetwork = w1 {
                self.title = currentNetwork.ssid
                switch currentNetwork.status {
                case .connectionFailed:
                    self.color = .red
                    self.icon = Image(systemName:"wifi.exclamationmark")
                    self.errorMessage = "Connection Failed"
                case .connectionLost:
                    self.color = .red
                    self.icon = Image(systemName:"wifi.exclamationmark")
                    self.errorMessage = "Connection Lost"
                case .disconnected:
                    self.color = .red
                    self.icon = Image(systemName:"wifi.exclamationmark")
                    self.errorMessage = "Disconnected"
                case .idle:
                    self.icon = Image(systemName:"wifi.slash")
                    self.color = .orange
                    self.errorMessage = "Idle"
                case .noSSID:
                    self.color = .red
                    self.icon = Image(systemName:"wifi.exclamationmark")
                    self.errorMessage = "No SSID"
                case .noShield:
                    self.color = .red
                    self.icon = Image(systemName:"wifi.slash")
                    self.errorMessage = "Wifi broken"
                case .scanCompleted:
                    self.color = .green
                    self.icon = Image(systemName:"wifi")
                    self.errorMessage = "Scan completed"
                    
                case .connected:
                    self.color = .green
                    self.icon = Image(systemName:"wifi")
                    self.errorMessage = nil
                }
            } else {
                self.icon = Image(systemName:"wifi")
                self.color = .orange
                self.title = "Checking network"
            }
        }.store(in: &bag)
//        let c = networkService.$currentNetwork.
    }
    

}

//extension CurrentNetwork {
//    var title: String {
//        if self.connected && self.ssid != "" {
//            return self.ssid
//        }
//        return "Not connected"
//    }
//
//    var icon: String {
//        switch connected {
//        case true: return "wifi"
//        case false: return "wifi.slash"
//        }
//    }
//}
struct NetworkSummaryView: View {
    
    @ObservedObject var viewModel: NetworkSummaryViewModel
    
    var body: some View {
        
        ConfigItemView(icon: viewModel.icon, iconColor: viewModel.color, title: viewModel.title) {
            self.viewModel.errorMessage.map{Text($0)}
        }
    }
}



struct NetworkSummaryView_Previews: PreviewProvider {

    
    static var previews: some View {
        Group{
            ForEach(WifiStatus.allCases){ status -> NetworkSummaryView in
                let networkService = NetworkService()
                networkService.currentNetwork = CurrentNetwork(status: status, connected: true, ssid: "Fishnet", channel: 5, ip: IPv4Address("192.167.234.12"), rssi: -20)
                let viewModel = NetworkSummaryViewModel(networkService)
                
                return NetworkSummaryView(viewModel: viewModel)
                
            }.previewLayout(.sizeThatFits)
        
        }
        
    }
}



