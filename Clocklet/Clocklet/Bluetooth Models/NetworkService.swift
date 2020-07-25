//
//  NetworkService.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 12/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CoreBluetooth
import CombineBluetooth
import Combine

class NetworkService: ServiceProtocol {
    
    var bag: [AnyCancellable] = []
    
    required init(){
        $currentNetwork.map{
            if let currentNetwork = $0 {
                return currentNetwork.configured ? .configured : .notConfigured
            } else {
                return .unknown
            }
        }
        .assign(to: \.isConfigured, on: self)
        .store(in: &bag)
    }
    
    @Published var isConfigured: ConfigState = .unknown
    
    @Characteristic("AF2B36C7-0E65-457F-A8AB-B996B656CF32") var availableNetworks: [AvailableNetwork]?
    @Characteristic("BEB5483E-36E1-4688-B7F5-EA07361B26A8") var currentNetwork: CurrentNetwork?
    @Characteristic("DFBDE057-782C-49F8-A054-46D19B404D9F") var joinNetwork: JoinNetwork?
    
    func joinNetwork(_ network: AvailableNetwork, psk: String? = nil) throws {
        joinNetwork = JoinNetwork(ssid: network.ssid, psk: psk, enctype: network.enctype)
    }
    
    
}
