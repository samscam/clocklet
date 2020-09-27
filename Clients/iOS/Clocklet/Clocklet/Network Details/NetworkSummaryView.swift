//
//  NetworkStatusView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 10/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import Network
import Combine
import CombineBluetooth

extension WifiStatus {
    var icon: Image {
        switch self{
        case .connectionFailed:
            return Image(systemName:"wifi.exclamationmark")
        case .connectionLost:
            return Image(systemName:"wifi.exclamationmark")
        case .disconnected:
            return Image(systemName:"wifi.exclamationmark")
        case .idle:
            return Image(systemName:"wifi.slash")
        case .noSSID:
            return Image(systemName:"wifi.exclamationmark")
        case .noShield:
            return Image(systemName:"wifi.slash")
        case .scanCompleted:
            return Image(systemName:"wifi")
        case .connected:
            return Image(systemName:"wifi")
        }
    }
    
    var title: String {
        switch self {
        case .connectionFailed:
            return "Connection Failed"
        case .connectionLost:
            return "Connection Lost"
        case .disconnected:
            return "Disconnected"
        case .idle:
            return "Idle"
        case .noSSID:
            return "Network unavailable"
        case .noShield:
            return "Wifi broken"
        case .scanCompleted:
            return "Scan completed"
        case .connected:
            return "Connected"
        }
    }
    
    var colour: Color{
        switch self {
        case .connectionFailed:
            return .red
        case .connectionLost:
            return .red
        case .disconnected:
            return .red
        case .idle:
            return .orange
        case .noSSID:
            return .red
        case .noShield:
            return .red
        case .scanCompleted:
            return .green
        case .connected:
            return .green
        }
    }
}

class NetworkSummaryViewModel: ObservableObject{
    @Published var title: String = ""
    @Published var errorMessage: String? = nil
    @Published var icon: Image = Image(systemName:"questionmark.circle")
    @Published var color: Color = .black
    @Published var isConfigured: ConfigState = .unknown
    private var bag = [AnyCancellable]()
    
    private var networkService: NetworkService
    
    init(_ networkService: NetworkService){
        self.networkService = networkService
        networkService.$currentNetwork.shouldNotify = true
        
        networkService.$isConfigured.sink { (configState) in
            self.isConfigured = configState
        }.store(in: &bag)
        
        networkService
            .$currentNetwork
            .sink(receiveCompletion: { (completion) in
                switch completion {
                case .failure(let error):
                    self.icon = Image(systemName:"wifi")
                    self.color = .red
                    self.title = "Error"
                    self.errorMessage = error.localizedDescription
                case .finished:
                    self.icon = Image(systemName:"wifi")
                    self.color = .red
                    self.title = "Completed"
                }
                
            }) { [weak self] (currentNetwork) in
                guard let self = self else {
                    return
                }
                
                guard let currentNetwork = currentNetwork else {
                    self.icon = Image(systemName:"wifi")
                    self.color = .orange
                    self.title = "Checking network"
                    return
                }
                
                
                self.title = currentNetwork.ssid ?? "No network set"
                
                
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
                    self.errorMessage = "Network unavailable"
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
                
        }.store(in: &bag)
    }
    
    deinit {
        networkService.$currentNetwork.shouldNotify = false
    }
}


struct NetworkSummaryView: View {
    @EnvironmentObject var viewModel: NetworkSummaryViewModel
    
    var body: some View {
        
        ConfigItemView(icon: viewModel.icon, iconColor: viewModel.color, title: viewModel.title) {
            
            Group{
                if (self.viewModel.isConfigured == ConfigState.notConfigured){
                    Text("Configure Network").lozenge()
                } else {
                    if (self.viewModel.errorMessage != nil){
                        Text(self.viewModel.errorMessage!).foregroundColor(viewModel.color)
                    }
                }
            }
        }
    }
}

struct NetworkHeaderView: View {
    @EnvironmentObject var viewModel: NetworkSummaryViewModel
    
    var body: some View {
        
        ConfigItemView(icon: viewModel.icon, iconColor: viewModel.color, title: viewModel.title) {
            
            Group{
                if (self.viewModel.errorMessage != nil){
                    Text(self.viewModel.errorMessage!)
                }
            }
        }
    }
}




struct NetworkSummaryView_Previews: PreviewProvider {
    
    
    static var previews: some View {
        
        Group{
            ForEach(WifiStatus.allCases){ status in
                
                return NetworkSummaryView()
                
            }.previewLayout(.sizeThatFits).environmentObject(NetworkService())
            
        }
        
    }
}



