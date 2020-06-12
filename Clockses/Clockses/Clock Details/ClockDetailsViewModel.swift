//
//  ClockDetailsViewModel.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 11/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import Combine
import SwiftUI
import CombineBluetooth
import CoreLocation
//
//class ClockDetailsViewModel: ObservableObject {
//    
//    
//    // Top level
//    @Published var title: String = ""
//    @Published var image: Image = Image(systemName:"questionmark.circle")
//    
//    // Connection
//    @Published var connectionIcon: Image = Image(systemName:ConnectionState.disconnected(error: nil).iconSystemName)
//    @Published var connectionMessage: String = ""
//    @Published var connectionErrorMessage: String? = nil
//    @Published var connectionColor: Color = .black
//
//    @Published var networkSummary: NetworkSummaryViewModel?
////    @Published var networkDetails: NetworkDetailsViewModel?
//    @Published var locationSummaryViewModel: LocationSummaryViewModel?
//    
//    var clock: Clock
//    
//    private var bag = [AnyCancellable]()
//    
//    init(clock: Clock){
//        self.clock = clock
//    }
//    
//    func onAppear(){
//        clock.connect()
//        self.image = Image(clock.caseColor.imageName)
//        
//        clock.$name.assign(to: \.title, on: self).store(in: &bag)
//        
//        clock.$state.sink(receiveValue: { connectionState in
//            
//            self.connectionIcon = Image(systemName:connectionState.iconSystemName)
//            self.connectionErrorMessage = nil
//            switch connectionState {
//            case .connected:
//                self.connectionMessage = "Connected"
//                self.connectionColor = .blue
//            case .connecting:
//                self.connectionMessage = "Connecting..."
//                self.connectionColor = .orange
//            case .disconnected(let error):
//                self.connectionColor = .red
//                if let error = error {
//                    self.connectionErrorMessage = error.localizedDescription
//                }
//                
//                self.connectionMessage = "Disconnected"
//                
//            }
//        }).store(in: &bag)
//        
//
//                
//    }
//    func onDisappear(){
////        bag = []
//    }
//
//}
//
