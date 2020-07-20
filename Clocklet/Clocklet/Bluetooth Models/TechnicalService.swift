//
//  TechnicalService.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 19/07/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CoreBluetooth
import CombineBluetooth
import Combine


class TechnicalService: ServiceProtocol {
    required init(){
    }
    
    static let uuid = CBUUID(string: "417BD398-B942-4FF1-A759-02409F17D994")
    
    @Characteristic(CBUUID(string:"DD3FB44B-A925-4FC3-8047-77B1B6028B25")) var reset: ResetType?
}



enum ResetType: Int, DataConvertible {
    case nothing = 0
    case reboot
    case partialReset
    case factoryReset
}
