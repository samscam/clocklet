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
    
    @StateObject private var central = Central()

    
    var body: some Scene {
        WindowGroup{
            MainView().environmentObject(central)
        }
    }
}

extension EnvironmentValues {
    @Entry var central: Central? = nil
}
