//
//  ClockListViewModel.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 10/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import Combine

// BEST LINK https://medium.com/ios-os-x-development/learn-master-%EF%B8%8F-the-basics-of-combine-in-5-minutes-639421268219


class ClockListViewModel: ObservableObject {
    @Published var isScanning = false
    @Published var clockConnections: [ClockConnection] = []
    
    var _cancellableScanning: Cancellable?
    var _cancellableConnections: Cancellable?
    
    let blue = Bluechatter.shared
    
    init(){
        _cancellableScanning = blue.isScanningSubject.assign(to: \.isScanning, on: self)
        _cancellableConnections = blue.clockConnections.map({$0.map({ $0.value })}).assign(to: \.clockConnections, on: self)
        
    }
    
    func startScanning(){
        // don't actually do anything...
    }
}
