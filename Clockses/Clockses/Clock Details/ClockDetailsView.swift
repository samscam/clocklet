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


struct ClockDetailsView: View {
    
    @ObservedObject var viewModel: ClockDetailsViewModel
    
    
    var body: some View {
        ScrollView{
            VStack(){
                viewModel.image.resizable().aspectRatio(contentMode: .fit).frame(width: nil, height: 200, alignment: .center)
                
                ConfigItemView(icon: viewModel.connectionIcon ,
                               iconColor: viewModel.connectionColor,
                               title: viewModel.connectionMessage) {
                                self.viewModel.connectionErrorMessage.map{ Text($0) }
                }
                
                viewModel.networkDetails.map{
                    NavigationLink(destination:NetworkDetailView(viewModel: $0)){
                        viewModel.networkSummary.flatMap{
                            NetworkSummaryView(viewModel: $0)
                        }
                    }.accentColor(nil)
                }
                
                viewModel.locationSummaryViewModel.map{
                    LocationSummaryView($0)
                }
                
            }
            .padding()
            .animation(.default)
            .onAppear {
                self.viewModel.onAppear()
                self.viewModel.connect()
            }.onDisappear {
                self.viewModel.onDisappear()
                //                self.viewModel.clock.disconnect()
            }
            
        }.navigationBarTitle(Text(viewModel.title), displayMode:.large)
    }
}

extension ContentSizeCategory{
    static func allCases() -> [ContentSizeCategory]{
        return self.allCases
    }
}

struct ClockDetailsView_Previews: PreviewProvider {
    
    static let viewModel: ClockDetailsViewModel = {
        var clock = Clock("Foop",.bare)
        
        clock.caseColor = .wood
        clock.networkService.currentNetwork = CurrentNetwork(status: .connected, connected: true, ssid: "Fishnet", channel: 1, ip: nil, rssi: -20)
        clock.locationService.currentLocation = CurrentLocation(lat: 53.431808, lng: -2.218080)
        var viewModel = ClockDetailsViewModel(clock: clock)
        viewModel.connectionErrorMessage = "This all went wrong today"
        return viewModel
    }()
    
    static var previews: some View {
        Group{
            ForEach(ContentSizeCategory.allCases, id: \.hashValue) { item  in
                
                NavigationView{
                    ClockDetailsView(viewModel: viewModel)
                }
                .environment(\.sizeCategory,item).previewDevice("iPhone SE")
                
            }
        }
    }
    //        ClockDetailsView(clock: )
    //        let clock = ClockModel(id: UUID(), serial: 5, name: "Clocklet #291", caseColor: .wood)
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

