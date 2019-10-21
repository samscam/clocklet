//
//  ClockDetailsViewModel.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 11/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import Combine

class ClockDetailsViewModel: ObservableObject {
    @Published var clock: Clock
    @Published var currentNetwork: CurrentNetwork?
    @Published var availableNetworks: [AvailableNetwork]?
    
    var connection: ClockConnection?
    
    private var cancellables = [AnyCancellable]()
    
    init(clockConnection: ClockConnection){
        self.connection = clockConnection
        self.clock = clockConnection.clock
        let c = clockConnection.objectWillChange.sink(receiveValue: { (_) in
            self.objectWillChange.send()
        })
        cancellables.append(c)
    }
    
    func connect(){
        connection?.connect()
    }
}
