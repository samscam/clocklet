//
//  Clock.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 21/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CombineBluetooth

fileprivate extension Data {

    init<T>(from value: T) {
        self = Swift.withUnsafeBytes(of: value) { Data($0) }
    }

    func to<T>(_ type: T.Type) -> T? where T: ExpressibleByIntegerLiteral {
        var value: T = 0
        guard count >= MemoryLayout.size(ofValue: value) else { return nil }
        _ = Swift.withUnsafeMutableBytes(of: &value, { copyBytes(to: $0)} )
        return value
    }
}

extension ManufacturerData {
    var hwRev: UInt16 {
        get{
            guard let residual = residual,
                residual.count >= 4 else {
                    return 0
            }
            
            return residual[2..<4].to(UInt16.self) ?? 0
        }
    }
    
    var caseColor: CaseColor {
        get{
            guard let residual = residual,
                residual.count >= 6 else {
                    return .bones
            }
            
            let ccNumber: UInt16 = residual[4..<6].to(UInt16.self) ?? 0
            return CaseColor(rawValue:ccNumber) ?? .bones

        }
    }
    var serial: UInt32 {
        get{
            guard let residual = residual,
                residual.count >= 10 else {
                    return 0
            }
            
            return residual[6..<10].to(UInt32.self) ?? 0
        }
    }

}

class Clock: Peripheral, Identifiable, Advertiser {
    
    var id: UUID {
        return uuid
    }
    
    
    private var _caseColor: CaseColor?
    var caseColor: CaseColor {
        
        get{
            if let storedCaseColour = _caseColor {
                return storedCaseColour
            }
            
            if let advertisedCaseColour = advertisementData?.manufacturerData?.caseColor {

                return advertisedCaseColour
            }
            
            return .bones
        }
        set {
            _caseColor = newValue
        }
    }
    
    var serial: UInt32 {
        get{
            return advertisementData?.manufacturerData?.serial ?? 0
        }
    }
    
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
    
    @Service var deviceInfoService: DeviceInfoService?
    @Service var technicalService: TechnicalService?
    
    static var advertised: [InnerServiceProtocol.Type] = [NetworkService.self]
}


enum CaseColor: UInt16, Codable {
    case bones = 0
    case wood = 1
    case translucent = 2
    case bluePink = 3
    case white = 4
    case black = 5
    
    
    var imageName: String {
        switch self{
        case .bones: return "r0-bones"
        case .wood: return "r5-wood"
        case .translucent: return "r5-translucent"
        case .bluePink: return "r5-bluepink"
        case .white: return "r5-white"
        case .black: return "r5-black"
        
        }
    }
}
