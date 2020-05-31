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

class NetworkDetailsViewModel: ObservableObject{
    @Published var networkSummary: NetworkSummaryViewModel?
    @Published var availableNetworks: [AvailableNetwork]?
    
    private var bag = [AnyCancellable]()
    
    fileprivate let networkService: NetworkService
    
    init(_ networkService: NetworkService){
        self.networkService = networkService
        networkSummary = NetworkSummaryViewModel(networkService)
        networkService
            .$availableNetworks
            .compactMap{$0}
            .catch({ (error) in
                return Just<[AvailableNetwork]?>(nil)
            })
            .assign(to: \.availableNetworks, on: self)
            .store(in: &bag)
    }
}

struct NetworkDetailView: View {
    
    @ObservedObject var viewModel: NetworkDetailsViewModel
    
    var body: some View {
        ScrollView{
            VStack{
                self.viewModel.networkSummary.map{
                    NetworkSummaryView(viewModel: $0)
                }
                
                self.viewModel.availableNetworks.map{
                    AvailableNetworksView($0,networkService: self.viewModel.networkService)
                }
                
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
        let viewModel = NetworkDetailsViewModel(networkService)
        
        return NetworkDetailView(viewModel: viewModel)
//        return NetworkDetailView()
    }
}
