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

class ClockDetailsViewModel: ObservableObject {
    
    // Top level
    @Published var title: String = ""
    @Published var image: Image = Image(systemName:"questionmark.circle")
    
    // Connection
    @Published var connectionIcon: Image = Image(systemName:ConnectionState.disconnected(error: nil).iconSystemName)
    @Published var connectionMessage: String = ""
    @Published var connectionErrorMessage: String? = nil
    @Published var connectionColor: Color = .black

    @Published var networkSummary: NetworkSummaryViewModel?
    @Published var networkDetails: NetworkDetailsViewModel?
    @Published var locationSummaryViewModel: LocationSummaryViewModel?
    
    private var clock: Clock
    
    private var bag = [AnyCancellable]()
    
    init(clock: Clock){
        self.clock = clock
        

    }
    
    func connect(){
        clock.connect()
    }
    
    func onAppear(){
        self.image = Image(clock.caseColor.imageName)
        
        clock.$name.assign(to: \.title, on: self).store(in: &bag)
        
        clock.$state.sink(receiveValue: { connectionState in
            
            self.connectionIcon = Image(systemName:connectionState.iconSystemName)
            self.connectionErrorMessage = nil
            switch connectionState {
            case .connected:
                self.connectionMessage = "Connected"
                self.connectionColor = .blue
            case .connecting:
                self.connectionMessage = "Connecting..."
                self.connectionColor = .orange
            case .disconnected(let error):
                self.connectionColor = .red
                if let error = error {
                    self.connectionErrorMessage = error.localizedDescription
                }
                
                self.connectionMessage = "Disconnected"
                
            }
        }).store(in: &bag)
        
        clock.$networkService
            .publisher
            .compactMap{$0}
            .map(NetworkSummaryViewModel.init)
            .assign(to: \.networkSummary, on: self)
            .store(in: &bag)
        
        clock.$networkService
            .publisher
            .compactMap{$0}
            .map(NetworkDetailsViewModel.init)
            .assign(to: \.networkDetails, on: self)
            .store(in: &bag)
        

                
        clock.$locationService
            .publisher
            .compactMap{$0}
            .map(LocationSummaryViewModel.init)
            .assign(to: \.locationSummaryViewModel, on: self)
            .store(in: &bag)
                
                
    }
    func onDisappear(){
        bag = []
    }

}

extension ConnectionState{
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
