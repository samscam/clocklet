//
//  SettingsService.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 06/05/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CoreBluetooth
import CombineBluetooth
import Combine


class SettingsService: ServiceProtocol {
    
    required init(){
    }
    
    static let uuid = CBUUID(string: "28C65464-311E-4ABF-B6A0-D03B0BAA2815")
    
    @Characteristic(CBUUID(string:"9982B160-23EF-42FF-9848-31D9FF21F490")) var availableSeparatorAnimations: [SeparatorAnimation]?
    
    @Characteristic(CBUUID(string:"2371E298-DCE5-4E1C-9CB2-5542213CE81C")) var separatorAnimation: SeparatorAnimation?


    
}

enum SeparatorAnimation: String, RawRepresentable, Identifiable, Codable {
    var id: String { return self.rawValue }
    
    case Static
    case Blink
    case Fade
}


extension SeparatorAnimation: DataConvertible {


    public init?(data: Data){
        guard let value = String(data: data, encoding: .utf8) else {
            return nil
        }
        self.init(rawValue:value)
    }

    public var data: Data {
        return Data(id.utf8)
    }
}

