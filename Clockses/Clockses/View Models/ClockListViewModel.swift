//
//  ClockListViewModel.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 10/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import Combine
import CombineBluetooth

// BEST LINK https://medium.com/ios-os-x-development/learn-master-%EF%B8%8F-the-basics-of-combine-in-5-minutes-639421268219


class ClockListViewModel: ObservableObject {
    @Published var isScanning = true
    @Published var clockConnections: [Connection] = []
    
    var clocks: [Clock] { return clockConnections.compactMap{$0.peripheral as? Clock} }
    
    var _cancellableScanning: Cancellable?
    var _cancellableClocks: Cancellable?
    
    let central = Central()
    
    init(){
        _cancellableScanning = central.$isScanning.assign(to: \.isScanning, on: self)
    }
    
    deinit{
        print("ClockListViewmodel deinit")
    }
    
    func startScanning(){
        _cancellableClocks = central.discoverConnections(for: Clock.self).assign(to: \.clockConnections, on: self)
    }
    func stopScanning(){
        _cancellableClocks = nil
    }
}
