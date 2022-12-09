//
//  ClockletApp.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 10/05/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import CombineBluetooth

@main
struct ClockletApp: App{
    let clockList = ClockListViewModel(central:Central())
    
    var body: some Scene {
        WindowGroup{ ClockListView().environmentObject(clockList)
        }
    }
}
