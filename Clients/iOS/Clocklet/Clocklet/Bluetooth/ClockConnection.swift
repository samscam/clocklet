////
////  ClockConnection.swift
////  Clockses
////
////  Created by Sam Easterby-Smith on 11/10/2019.
////  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
////
//
//import Foundation
//import CoreBluetooth
//
//
////
//class ClockConnection: NSObject, Identifiable, ObservableObject {
//    var id: UUID { return peripheral?.identifier ?? UUID() }
//    
//    // Top level data
//    @Published var clock: ClockModel
//    @Published var rssi: Int
//    @Published var state: ConnectionState = .disconnected
//    
//    // Services
//    @Published var networkService: NetworkService? = nil
//    
//    let peripheral: CBPeripheral?
//    
//    init(peripheral: CBPeripheral, rssi: Int, clock: ClockModel){
//        
//        self.peripheral = peripheral
//        self.rssi = rssi
//        self.clock = clock
//        
//        super.init()
//        
//        self.peripheral?.delegate = self
//    }
//    
//    // For creating previews without an actual peripheral
//    init(clock: ClockModel, networkService: NetworkService? = nil){
//        self.clock = clock
//        self.networkService = networkService
//        self.rssi = -1
//        self.peripheral = nil
//        self.state = .connected
//    }
//    
//    
//    public func peripheralStateChanged(){
//        guard let peripheral = self.peripheral else { return } // There should always be a peripheral unless we are testing
//        
//        switch peripheral.state {
//        case .connected:
//            state = .connected
//            peripheral.discoverServices(nil)
//        case .connecting:
//            state = .connecting
//        case .disconnected:
//            state = .disconnected
//        case .disconnecting:
//            break
//        @unknown default:
//            break
//        }
//    }
//
//    public func connect(){
//        if state != .connected {
//            state = .connecting
//            Bluechatter.shared.connect(to: self)
//        }
//    }
//    
//    public func disconnect(){
//        if state == .connected {
//            Bluechatter.shared.disconnect(from: self)
//        }
//    }
//

//    
//    
//}
//
//
//extension ClockConnection: CBPeripheralDelegate {
//    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
//        
//        // ERRORS??? nah
//        
//        guard let services = peripheral.services else { return }
//        let characteristicUUIDs: [CBUUID] = [AvailableNetwork.uuid,CurrentNetwork.uuid,JoinNetwork.uuid]
//        for service in services {
//            peripheral.discoverCharacteristics(characteristicUUIDs, for: service)
//        }
//        
//        // Fish out the network service
//        if let cbNetworkService = peripheral.services?.first(where: { (service) -> Bool in
//            service.uuid == NetworkService.uuid
//        }) {
//            // And put it in our nice little wrapper
//            networkService = NetworkService()
//            networkService?.service = cbNetworkService
//        }
//        
//    }
//    
//    func peripheral(_ peripheral: CBPeripheral, didModifyServices invalidatedServices: [CBService]) {
//        // modified services
//        print("Modified services")
//    }
//    
//    func peripheralDidUpdateName(_ peripheral: CBPeripheral) {
//        // updated name
//        
//        print("Updated name")
//        if let name = peripheral.name {
//            clock.name = name
//            self.objectWillChange.send()
//        }
//    }
//    
//    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
//        
//        guard let characteristics = service.characteristics else {return}
//        guard let networkService = networkService else { return }
//        guard service == networkService.service else { return }
//        
//        for characteristic in characteristics {
//            peripheral.setNotifyValue(true, for: characteristic)
//            print("Discovered char: ", characteristic)
//            peripheral.readValue(for: characteristic)
//        }
//    }
//    
//    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
//        print("Updated: ",characteristic)
//        
//        switch characteristic.service {
//        case networkService?.service:
//            networkService?.updateValue(for: characteristic)
//        default:
//            // unknown service, do nothing
//            break
//        }
//        
//        self.objectWillChange.send()
//
//    }
//    
//    
//    func peripheral(_ peripheral: CBPeripheral, didWriteValueFor characteristic: CBCharacteristic, error: Error?) {
//        // written
//    }
//    func peripheral(_ peripheral: CBPeripheral, didReadRSSI RSSI: NSNumber, error: Error?) {
//        // rssi
//    }
//    
//    
//}
//
