//
//  SettingsService.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 06/05/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CoreBluetooth
import CombineBluetooth
import Combine


class SettingsService: ServiceProtocol {
    
    required init(){}
    let objectWillChange = ObservableObjectPublisher() // If we leave it to the synthesised one it doesn't work :/
    
    static let uuid = CBUUID(string: "28C65464-311E-4ABF-B6A0-D03B0BAA2815")
    
    @Characteristic(CBUUID(string:"9982B160-23EF-42FF-9848-31D9FF21F490")) var availableSeparatorAnimations: String?
    
//    @Characteristic(CBUUID(string:"2371E298-DCE5-4E1C-9CB2-5542213CE81C")) var separatorAnimation: String?
    
    
}
