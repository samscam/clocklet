//
//  NetworkService.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 12/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CoreBluetooth
import CombineBluetooth

class NetworkService: ServiceProtocol, ObservableObject {
    static let uuid = CBUUID(string: "68D924A1-C1B2-497B-AC16-FD1D98EDB41F")
    
//    @Characteristic(CBUUID(string: "AF2B36C7-0E65-457F-A8AB-B996B656CF32")) var availableNetworks: [AvailableNetwork] = []
    
    @Characteristic(CBUUID(string: "BEB5483E-36E1-4688-B7F5-EA07361B26A8")) var currentNetwork: CurrentNetwork? = nil
    
    @Characteristic(CBUUID(string: "B2313F3F-0FE4-4EC2-B0B4-D978496CD2D9")) var joinNetwork: JoinNetwork? = nil
    
        func joinNetwork(_ network: AvailableNetwork, psk: String? = nil) throws {

        }
    
    func update(){

    }
}


//        func joinNetwork(_ network: AvailableNetwork, psk: String? = nil) throws {
//            guard let joinCharacteristic = service?.characteristics?.first(where:{$0.uuid == JoinNetwork.uuid}) else {
//                print("Could not find the JoinNetwork characteristic")
//                throw(NetworkServiceError.couldNotFindJoinCharacteristic)
//            }
//
//            let encoder = JSONEncoder()
//            let data = try encoder.encode(JoinNetwork(ssid: network.ssid, psk: psk, enctype: network.enctype))
//            print(String(data: data, encoding: .utf8) ?? "NADA")
//
//            service?.peripheral.writeValue(data, for: joinCharacteristic, type: .withResponse)
//        }
//
//class NetworkServiceTwo: ObservableObject {
//    static let uuid = CBUUID(string: "68D924A1-C1B2-497B-AC16-FD1D98EDB41F")
//    
//    @Published var availableNetworks: [AvailableNetwork]? = nil
//    @Published var currentNetwork: CurrentNetwork? = nil
//    
//    var service: CBService?
//    
//    func updateValue(for characteristic: CBCharacteristic){
//        
//        guard let data = characteristic.value else { return }// should arguably dump if nil but hey...
//        
//        let decoder = JSONDecoder()
//        
//        switch characteristic.uuid {
//        case AvailableNetwork.uuid:
//            do {
//                let av = try decoder.decode([AvailableNetwork].self, from: data)
//                availableNetworks = av.sorted(by: { (left, right) -> Bool in
//                    return left.ssid < right.ssid
//                })
//            } catch {
//                print(error)
//                print(String(data: data, encoding: .utf8) ?? "No data")
//            }
//        case CurrentNetwork.uuid:
//            do {
//                currentNetwork = try decoder.decode(CurrentNetwork.self, from: data)
//            } catch {
//                print(error)
//                print(String(data: data, encoding: .utf8) ?? "No data")
//            }
//        default:
//            break
//        }
//    }
//    
//    func joinNetwork(_ network: AvailableNetwork, psk: String? = nil) throws {
//        guard let joinCharacteristic = service?.characteristics?.first(where:{$0.uuid == JoinNetwork.uuid}) else {
//            print("Could not find the JoinNetwork characteristic")
//            throw(NetworkServiceError.couldNotFindJoinCharacteristic)
//        }
//        
//        let encoder = JSONEncoder()
//        let data = try encoder.encode(JoinNetwork(ssid: network.ssid, psk: psk, enctype: network.enctype))
//        print(String(data: data, encoding: .utf8) ?? "NADA")
//        
//        service?.peripheral.writeValue(data, for: joinCharacteristic, type: .withResponse)
//    }
//    
//    enum NetworkServiceError: Error {
//        case couldNotFindJoinCharacteristic
//    }
//}
