//
//  ClockList.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 10/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import Combine
import CombineBluetooth
import SwiftUI


class ClockList: ObservableObject {
    
    @Published var isScanning = true
    @Published var clocks: [Clock] = []
    
    
    var _cancellableScanning: Cancellable?
    var _cancellableClocks: Cancellable?
    
    let central: Central?
    
    init(central: Central?){
        self.central = central
        self._cancellableScanning = central?.$isScanning.assign(to: \.isScanning, on: self)
    }
    
    deinit{
        print("ClockListViewmodel deinit")
    }
    
    func startScanning(){
        guard let central = central else {
            return
        }
        _cancellableClocks = central
            .discoverConnections(for: Clock.self)
            .map{ $0.compactMap{ $0.peripheral as? Clock } }
            .assign(to: \.clocks, on: self)
    }
    
    func stopScanning(){
        _cancellableClocks?.cancel()
        _cancellableClocks = nil
    }
    
    func disconnectAllDevices(){
        central?.disconnectAllDevices()
    }
    
}
