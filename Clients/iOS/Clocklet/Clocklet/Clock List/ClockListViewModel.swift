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
    
    let bluetoothStatusViewModel: BluetoothStatusViewModel
    
    var bag = Set<AnyCancellable>()
    
    var _cancellableScanning: Cancellable?
    var _cancellableClocks: Cancellable?
    
    private var central: Central?
    
    init(central: Central?){
        self.central = central
        self.bluetoothStatusViewModel = BluetoothStatusViewModel(central: central)
        
        self._cancellableScanning = central?.$isScanning.assign(to: \.isScanning, on: self)
        
        central?.$state
            .map{ newState in
                switch newState {
                case .poweredOn, .unknown:
                    return false
                default:
                    return true
                }
            }
            .assign(to: \.showBluetoothOverlay, on: self)
            .store(in: &bag)
    }
    
    deinit{
        print("ClockListViewModel deinit")
    }
    
    func startScanning(){
        guard let central = central else {
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
