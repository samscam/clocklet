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
        
        // The available networks characteristic will continually spam us with networks as they are found or updated.
        $availableNetworks
            .compactMap { $0 }
            // Deduplicate by BSSID
            .scan([String: AvailableNetwork]()) { (prev, newValue) -> [String: AvailableNetwork] in
                var prev = prev
                prev[newValue.bssid] = newValue
                return prev
            }
            .map{
                $0.values.sorted{ $0.ssid < $1.ssid }.reduce([AvailableNetwork]()) { (result, network) -> [AvailableNetwork] in
                    var result = result
                    if (result.contains{ $0.ssid == network.ssid && $0.rssi < network.rssi }){
                        // Replace the weaker network with the one we just found
                        var filtered = result.filter{ $0.ssid != network.ssid}
                        filtered.append(network)
                        return filtered
                    } else  if (!result.contains{ $0.ssid == network.ssid}) {
                        result.append(network)
                    }
                    return result
                }
                    
            }
            .assign(to: \.scannedNetworks, on: self).store(in: &bag)
    }
    
    @Published var isConfigured: ConfigState = .unknown
    @Published var scannedNetworks: [AvailableNetwork] = .init()
    
    @Characteristic("AF2B36C7-0E65-457F-A8AB-B996B656CF32") var availableNetworks: AvailableNetwork?
    
    @Characteristic("BEB5483E-36E1-4688-B7F5-EA07361B26A8") var currentNetwork: CurrentNetwork?
    
    @Characteristic("DFBDE057-782C-49F8-A054-46D19B404D9F") var joinNetwork: JoinNetwork?
    
    func joinNetwork(_ network: AvailableNetwork, psk: String? = nil) {
        joinNetwork = JoinNetwork(ssid: network.ssid, psk: psk, enctype: network.enctype)
    }
    
    
}
