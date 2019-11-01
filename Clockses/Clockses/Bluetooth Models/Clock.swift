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
    
    var caseColor: CaseColor = .black
    var serial: UInt32 = 4242
    
    init(_ name: String, _ color: CaseColor = .black){
        super.init(uuid: UUID(), name: name)
        self.caseColor = color
    }
    
    required init(uuid: UUID, name: String, connection: Connection) {
        super.init(uuid:uuid, name: name, connection: connection)
    }
    
    
    @Service var networkService: NetworkService = NetworkService()
    @Service var locationService: LocationService = LocationService()
    
    static var advertised: [InnerServiceProtocol.Type] = [NetworkService.self]
}


enum CaseColor: String, Codable {
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
