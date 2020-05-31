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
import SwiftUI

// BEST LINK https://medium.com/ios-os-x-development/learn-master-%EF%B8%8F-the-basics-of-combine-in-5-minutes-639421268219


class ClockListViewModel: ObservableObject {
    @Published var isScanning = true
    @Published var clockItems: [ClockSummaryViewModel] = []
    
    
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
        _cancellableClocks = central
            .discoverConnections(for: Clock.self)
            .map{ $0.compactMap{ $0.peripheral as? Clock } }
            .map{ $0.map(ClockSummaryViewModel.init) }
            .assign(to: \.clockItems, on: self)
    }
    
    func stopScanning(){
        _cancellableClocks?.cancel()
        _cancellableClocks = nil
    }
    
}

class ClockSummaryViewModel: ObservableObject, Identifiable {
    var id: UUID {
        return clock.id
    }
    
    @Published var image: Image
    @Published var title: String = ""
    var bag: [AnyCancellable] = []
    
    lazy var detailsViewModel = ClockDetailsViewModel(clock: self.clock)
    
    private let clock: Clock
    
    init(clock: Clock){
        self.clock = clock
        
        image = Image(clock.caseColor.imageName)
        
        clock.$name.assign(to: \.title, on: self).store(in: &bag)
    }
}
