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
import Combine

class NetworkService: ServiceProtocol {
    
    required init(){}
    
    let objectWillChange = ObservableObjectPublisher() // If we leave it to the synthesised one it doesn't work :/
    
    static let uuid = CBUUID(string: "68D924A1-C1B2-497B-AC16-FD1D98EDB41F")
    
    @Characteristic(CBUUID(string: "AF2B36C7-0E65-457F-A8AB-B996B656CF32")) var availableNetworks: [AvailableNetwork]? = nil
    
    @Characteristic(CBUUID(string: "BEB5483E-36E1-4688-B7F5-EA07361B26A8")) var currentNetwork: CurrentNetwork? = nil
    
    @Characteristic(CBUUID(string: "DFBDE057-782C-49F8-A054-46D19B404D9F")) var joinNetwork: JoinNetwork? = nil
    
    func joinNetwork(_ network: AvailableNetwork, psk: String? = nil) throws {
        joinNetwork = JoinNetwork(ssid: network.ssid, psk: psk, enctype: network.enctype)
    }
    
}
