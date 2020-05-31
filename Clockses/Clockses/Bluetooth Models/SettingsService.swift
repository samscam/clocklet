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
    
    @Characteristic(CBUUID(string:"256543AE-AA83-4A95-9347-7C7DFDD98B71")) var twelveHour: Bool?
    
    @Characteristic(CBUUID(string:"9982B160-23EF-42FF-9848-31D9FF21F490")) var flashDots: Bool?
    
}
