//
//  Clock.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 21/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CombineBluetooth

class Clock: Peripheral, Identifiable, Advertiser {
    
    var id: UUID {
        return uuid
    }
    
    var caseColour: CaseColour = .black
    var serial: UInt32 = 4242
    
    @Service var networkService: NetworkService = NetworkService()
    @Service var locationService: LocationService = LocationService()
    
    static var advertised: [ServiceProtocol.Type] = [NetworkService.self]
}


enum CaseColour: String, Codable {
    case bare
    case black
    case wood
    
    var imageName: String {
        switch self{
        case .bare: return "esp32feather"
        case .black: return "black-clocklet"
        case .wood: return "wood-clocklet"
        }
    }
}
