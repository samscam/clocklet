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
    @Characteristic("3499658F-6794-4690-8728-EB96BFFD01FA") var autoUpdates: Bool?
    
    lazy var stagingSelected = Binding<Bool>(
        get:{ return self.staging ?? false },
        set:{ self.staging = $0 }
    )
    
    lazy var autoUpdatesSelected = Binding<Bool>(
        get:{ return self.autoUpdates ?? true },
        set:{ self.autoUpdates = $0 }
    )
}



enum ResetType: Int, DataConvertible {
    case nothing = 0
    case reboot
    case partialReset
    case factoryReset
}
