//
//  DeviceInfoService.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 20/07/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CoreBluetooth
import CombineBluetooth
import Combine


class DeviceInfoService: ServiceProtocol {
    required init(){
    }
    
    static let uuid = CBUUID(string: "180A")
    
    @Characteristic(CBUUID(string: "2A29")) var manufacturerName: String?
    @Characteristic(CBUUID(string: "2A24")) var model: String?
    @Characteristic(CBUUID(string: "2A25")) var serialNumber: String?
    @Characteristic(CBUUID(string: "2A26")) var firmwareVersion: String?
}
