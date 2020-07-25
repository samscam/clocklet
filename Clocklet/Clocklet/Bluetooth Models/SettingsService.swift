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
import SwiftUI

class SettingsService: ServiceProtocol {
    
    required init(){
    }
    
    @Characteristic("9982B160-23EF-42FF-9848-31D9FF21F490") var availableSeparatorAnimations: [String]?
    
    @Characteristic("2371E298-DCE5-4E1C-9CB2-5542213CE81C") var separatorAnimation: String?

    @Characteristic("698D2B57-5B54-48D7-A483-1AB4660FBAF9") var availableTimeStyles: [String]?
    
    @Characteristic("AE35C2DE-7D36-4699-A5CE-A0FA6A0A5483") var timeStyle: String?
    
    
    lazy var selectedTimeStyle = Binding<String>(
        get:{ return self.timeStyle ?? "None" },
        set:{ self.timeStyle = $0 }
    )
    
    lazy var selectedAnimation = Binding<String>(
        get:{ return self.separatorAnimation ?? "Blink"},
        set:{ self.separatorAnimation = $0 }
    )
}

extension SettingsService {
    var timeStyles: [String] { get {
        return self.availableTimeStyles ?? []
    }}
    
    var separatorAnimations: [String] { get {
        return self.availableSeparatorAnimations ?? []
    }}
        
}



enum SeparatorAnimation: String, RawRepresentable, Identifiable, Codable {
    var id: String { return self.rawValue }
    
    case None
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

