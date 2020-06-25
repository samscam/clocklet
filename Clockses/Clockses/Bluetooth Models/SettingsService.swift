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
    
    var bag: [AnyCancellable] = []
    
    required init(){
    }
    
    static let uuid = CBUUID(string: "28C65464-311E-4ABF-B6A0-D03B0BAA2815")
    
    @Characteristic(CBUUID(string:"9982B160-23EF-42FF-9848-31D9FF21F490")) var availableSeparatorAnimations: [String]?
    
    @Characteristic(CBUUID(string:"2371E298-DCE5-4E1C-9CB2-5542213CE81C")) var separatorAnimation: String?
    
    @Published var separatorAnimationSelection: String = "Static" {
        didSet{
            separatorAnimation = separatorAnimationSelection
        }
    }
    
    
}


//struct StringOption: Identifiable, DataConvertible, JSONCharacteristic {
//    init(_ string: String) {
//        id = string
//    }
//
//    var id: String
//
//    public init?(data: Data){
//        guard let value = String(data: data, encoding: .utf8) else {
//            return nil
//        }
//
//        id = value
//    }
//
//    public var data: Data {
//        return Data(id.utf8)
//    }
//}
//
//extension StringOption: Codable {
//
//}
//
//extension StringOption: Hashable{
//
//}
