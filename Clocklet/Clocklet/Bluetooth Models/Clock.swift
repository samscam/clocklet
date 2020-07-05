//
//  Clock.swift
//  Clocklet
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
    
    
    private var _caseColor: CaseColor = .black
    var caseColor: CaseColor {
        get{
        guard let mfrResidual = advertisementData?.manufacturerData?.residual,
            let mfrString = String(data:mfrResidual, encoding: .utf8),
            let lastComponent = mfrString.split(separator: ",").last
        else {
                return _caseColor
        }
        
        return CaseColor(rawValue: String(lastComponent)) ?? _caseColor
        }
        set {
            _caseColor = newValue
        }
    }
    
    var serial: UInt32 = 4242
    
    init(_ name: String, _ color: CaseColor = .black){
        super.init(uuid: UUID(), name: name)
        self.caseColor = color
    }
    
    required init(uuid: UUID, name: String, connection: Connection) {
        super.init(uuid:uuid, name: name, connection: connection)
    }
    
    
    @Service var networkService: NetworkService?
    @Service var locationService: LocationService?
    @Service var settingsService: SettingsService?
    
    static var advertised: [InnerServiceProtocol.Type] = [NetworkService.self]
}


enum CaseColor: String, Codable {
    case bones
    case black
    case blue
    case wood
    
    var imageName: String {
        switch self{
        case .bones: return "esp32feather"
        case .black: return "clocklet5-blue-hello"
        case .blue: return "clocklet5-blue-hello"
        case .wood: return "wood-clocklet"
        }
    }
}
