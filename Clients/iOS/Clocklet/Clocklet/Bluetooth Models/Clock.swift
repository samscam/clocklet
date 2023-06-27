//
//  Clock.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 21/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CombineBluetooth
import Combine
import CoreBluetooth
import UIKit

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
    
    var caseColor: Clock.CaseColor {
        get{
            guard let residual = residual,
                residual.count >= 6 else {
                    return .bones
            }
            
            let ccNumber: UInt16 = residual[4..<6].to(UInt16.self) ?? 0
            return Clock.CaseColor(rawValue:ccNumber) ?? .bones

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

class Clock: Peripheral {
    

    
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
    
    private var _hwRev: UInt16?
    var hwRev: UInt16 {
        
        get{
            if let storedHwRev = _hwRev {
                return storedHwRev
            }
            
            if let advertisedHwRev = advertisementData?.manufacturerData?.hwRev {
                return advertisedHwRev
            }
            
            return 5
        }
        set {
            _hwRev = newValue
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
        monitorConfigState()
    }
    
    required init(uuid: UUID, name: String, connection: Connection) {
        super.init(uuid:uuid, name: name, connection: connection)
        monitorConfigState()
    }
    
    func monitorConfigState(){
        let networkServiceState: AnyPublisher<ConfigState, Never> = $networkService
            .replaceError(with: nil)
            .compactMap { $0 }
            .flatMap{ $0.$isConfigured }
            .print("Network service state")
            .eraseToAnyPublisher()
        
        let locationServiceState: AnyPublisher<ConfigState, Never> = $locationService
            .replaceError(with: nil)
            .compactMap { $0 }
            .flatMap{ $0.$isConfigured }
            .print("Location service state")
            .eraseToAnyPublisher()
        
        
        Publishers.CombineLatest(networkServiceState,locationServiceState)
            .map{ netConfigured, locConfigured in
                return netConfigured && locConfigured
            }
            .assign(to: \.isConfigured, on: self)
            .store(in: &bag)
    }
    
    @Service("180A") var deviceInfoService: DeviceInfoService?
    @Service("417BD398-B942-4FF1-A759-02409F17D994") var technicalService: TechnicalService?
    @Service("68D924A1-C1B2-497B-AC16-FD1D98EDB41F") var networkService: NetworkService?
    @Service("87888F3E-C1BF-4832-9823-F19C73328D30") var locationService: LocationService?
    @Service("28C65464-311E-4ABF-B6A0-D03B0BAA2815") var settingsService: SettingsService?
    @Service("03205794-6A59-42E5-9B8D-BB3879716FD5") var godModeService: GodModeService?
    
    @Published var isConfigured: ConfigState = .unknown


    enum CaseColor: UInt16, Codable, CustomStringConvertible {

        case bones = 0
        case wood = 1
        case translucent = 2
        case bluePink = 3
        case white = 4
        case black = 5
        case gold = 6
        case tequilla = 7
        
        var description: String {
            switch self {
            case .bones: return "bones"
            case .wood: return "wood"
            case .translucent: return "translucent"
            case .bluePink: return "bluepink"
            case .white: return "white"
            case .black: return "black"
            case .gold: return "gold"
            case .tequilla: return "tequilla"
            }
        }
        
    }

    var caseImage: UIImage {
        
        let imageName = "r\(hwRev)-\(caseColor)"
        return UIImage(named:imageName) ?? UIImage(named:"r0-bones")!
    }
    

}

extension Clock: Identifiable {
  var id: UUID {
      return uuid
  }
}

extension Clock: AdvertisementMatcher {
  static let advertisedServiceUUIDs: [String]? = ["68D924A1-C1B2-497B-AC16-FD1D98EDB41F"]
  
  static let advertisedManufacturer: String? = nil

}
