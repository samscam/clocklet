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
import SwiftUI

class TechnicalService: ServiceProtocol {
    required init(){
    }
    @Characteristic("DD3FB44B-A925-4FC3-8047-77B1B6028B25") var reset: ResetType?
    @Characteristic("78FEC95A-61A9-4C1C-BDA1-F79245E118C0") var staging: Bool?
    
    lazy var stagingSelected = Binding<Bool>(
        get:{ return self.staging ?? false },
        set:{ self.staging = $0 }
    )
}



enum ResetType: Int, DataConvertible {
    case nothing = 0
    case reboot
    case partialReset
    case factoryReset
}
