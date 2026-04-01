//
//  MainView.swift
//  Clocklet
//
//  Created by Sam on 29/03/2026.
//  Copyright © 2026 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import CombineBluetooth

struct MainView: View {
    @EnvironmentObject var central: Central
    
    var body: some View {
        
            switch central.state {
            case .poweredOn:
                ClockListView()
            default:
                BluetoothOverlayView()
                    .edgesIgnoringSafeArea(.all)
            }
        
    }
}

