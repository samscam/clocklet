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
import CoreBluetooth




class ClockListViewModel: ObservableObject {
    
    @Published var showBluetoothOverlay: Bool = false
    @Published var isScanning = true
    @Published var clocks: [Clock] = []
    @Published var selectedClock: Clock?
    
    
    var bag = Set<AnyCancellable>()
    
    var _cancellableScanning: Cancellable? = nil
    var _cancellableClocks: Cancellable? = nil
    
    var central: Central?{
        didSet{
            self._cancellableScanning = central?.$isScanning.assign(to: \.isScanning, on: self)
        }
    }
    
    init(){
    }
    
    deinit{
        print("ClockListViewModel deinit")
    }
    
    func startScanning(){
        guard let central else {
            return
        }
        _cancellableClocks = central
            .discoverConnections(matching: Clock.self)
            .timeout(10, scheduler: DispatchQueue.main)
            .map{ $0.compactMap{ $0.peripheral as? Clock } }
            .assign(to: \.clocks, on: self)
    }
    
    func stopScanning(){
        _cancellableClocks?.cancel()
        _cancellableClocks = nil
    }
    
    func toggleScanning(){
        if isScanning {
            stopScanning()
        } else {
            startScanning()
        }
    }
    
    func disconnectAllDevices(){
        central?.disconnectAllDevices()
    }
    
    func createFakeClock(){
        
        let fakeClock = Clock("Mock Clock", .bluePink)
        fakeClock.mockClockBehaviour()
        
        central = nil
        _cancellableClocks?.cancel()
        _cancellableScanning?.cancel()
        isScanning = false
        
        clocks = [fakeClock]
    }
    
}
