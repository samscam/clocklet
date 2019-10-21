//
//  Bluechatter.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 10/05/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CoreBluetooth
import Combine


protocol Characteristic {
    static var uuid: CBUUID { get }
}

class Bluechatter: NSObject {
    
    static let shared = Bluechatter()
    
    let cbcm = CBCentralManager.init()
    
    let clockConnections = CurrentValueSubject<[UUID:ClockConnection],Never>([:])
    let isScanningSubject = CurrentValueSubject<Bool, Never>(false)
    
    
    override init() {
        super.init()
        cbcm.delegate = self
    }
    

    func startScanning(){
        cbcm.scanForPeripherals(withServices: [NetworkService.uuid], options: nil)
    }
    
    func connect(to clockConnection: ClockConnection){
        if let peripheral = clockConnection.peripheral {
        cbcm.connect(peripheral, options: nil)
        }
    }
    
    func disconnect(from clockConnection: ClockConnection){
        if let peripheral = clockConnection.peripheral {
            cbcm.cancelPeripheralConnection(peripheral)
        }
    }
}

extension Bluechatter: CBCentralManagerDelegate {
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .poweredOn:
            isScanningSubject.value = true

            startScanning()
        default:
            print(central.state)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi: NSNumber) {
        print("Identifier: ", peripheral.identifier)
        print("Name: ", peripheral.name ?? "no name")
        print("Services: ", peripheral.services ?? "no services")
        print(advertisementData)
        
        var serial: UInt32?
        var caseColour: CaseColour?
        
        if let manData = advertisementData[CBAdvertisementDataManufacturerDataKey] as? Data {
            if let manString = String(data:manData, encoding: .ascii){
                print("Man data: ",manString)
                let components = manString.split(separator: ",")
                serial = UInt32(String(components[0]))
                caseColour = CaseColour(rawValue:String(components[1]))
            }
        }
        print(rssi)

        
        let localName = advertisementData[CBAdvertisementDataLocalNameKey] as? String
        let bestName = localName ?? peripheral.name ?? "Really no name"
        print("Best name: ",bestName)
        
        let clock = Clock(id: peripheral.identifier, serial: serial ?? 0, name: bestName, caseColour: caseColour ?? .bare)
        
        if let existingConnection = clockConnections.value[peripheral.identifier] {
            print("got an existing connection - we could update it...", existingConnection)
        } else {
            let connection = ClockConnection(peripheral: peripheral, rssi: Int(truncating:rssi), clock: clock)
            clockConnections.value[peripheral.identifier] = connection
        }
        print("---")
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        if let connection = clockConnections.value[peripheral.identifier] {
            connection.peripheralStateChanged()
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        // we lost it!
        if let connection = clockConnections.value[peripheral.identifier] {
            connection.peripheralStateChanged()
        }
    }
    
}
