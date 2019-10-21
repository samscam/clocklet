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
//    @Published var clockModel: ClockModel?
//    @Published var currentNetwork: CurrentNetwork?
//    @Published var availableNetworks: [AvailableNetwork]?
    
    @Published var clock: Clock?
    
    private var cancellables = [AnyCancellable]()
    
    init(clock: Clock){
        self.clock = clock
        let c = clock.objectWillChange.sink(receiveValue: { (_) in
            self.objectWillChange.send()
        })
        cancellables.append(c)
    }
    
    func connect(){
        clock?.connect()
    }
}
