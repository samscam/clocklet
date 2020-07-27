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

class Clock: Peripheral, Identifiable, AdvertisementMatcher {

    
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
    
    
    static var advertisedServiceUUIDs = ["68D924A1-C1B2-497B-AC16-FD1D98EDB41F"]
    
    @Published var isConfigured: ConfigState = .unknown
}

enum ConfigState: String {
    case unknown
    case notConfigured
    case configured
    
    static func &&(lhs: ConfigState, rhs: ConfigState)->ConfigState{
        switch (lhs, rhs){
        case (.unknown,_), (_,.unknown):
            return .unknown
        case (.notConfigured,_), (_,.notConfigured):
            return .notConfigured
        case (.configured, .configured):
            return .configured
        }
    }
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
