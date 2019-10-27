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
    @Published var locationServiceViewModel: LocationServiceViewModel?
    
    private var cancellables = [AnyCancellable]()
    
    init(clock: Clock){
        self.clock = clock
        
        let c = clock.objectWillChange.sink(receiveValue: { _ in
            self.objectWillChange.send()
        })
        cancellables.append(c)
        
        let c2 = clock.$locationService.publisher.compactMap{ $0 }.map{ LocationServiceViewModel($0)}.assign(to: \.locationServiceViewModel, on: self)
        cancellables.append(c2)
        
        
    }
    
    func connect(){
        clock.connect()
    }
}
