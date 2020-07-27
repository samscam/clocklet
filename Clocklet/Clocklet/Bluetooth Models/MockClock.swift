//
//  MockClock.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 17/07/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CoreLocation
import Network
import CombineBluetooth



class MockClock: Clock {
    init(){
        super.init("Mock Clocklet", .wood)
        
        // Sequence of events and behaviours for the mock clock

    }
    
    override func willConnect() {
        let sequence: [TimeInterval: (()->Void)] =
            [1: { self.state = .connected },
             2: {
                let networkService = NetworkService()
                
                networkService.currentNetwork = CurrentNetwork(status: .disconnected, connected: false, configured: false, ssid: nil, channel: 5, ip: nil, rssi: -10)
                
                networkService.availableNetworks = [
                    AvailableNetwork(ssid: "One network", enctype: .wpa2psk, rssi: -30, channel: 5, bssid: "bssone"),
                    AvailableNetwork(ssid: "Two network", enctype: .open, rssi: -30, channel: 5, bssid: "bsstwo"),
                    AvailableNetwork(ssid: "Three network", enctype: .wpa2psk, rssi: -80, channel: 6, bssid: "bssthree"),
                    AvailableNetwork(ssid: "Four network", enctype: .wpa2psk, rssi: -50, channel: 8, bssid: "bssfour"),
                    AvailableNetwork(ssid: "Five network", enctype: .wpa2psk, rssi: -20, channel: 5, bssid: "bssfive")
                    
                ]
                self.networkService = networkService
                
                },
             3:      {   self.locationService = LocationService()
             self.locationService?.currentLocation = CurrentLocation(configured: false, lat:  0, lng: 0)
                }
        ]
        sequence.forEach { (timeInterval, closure) in
            DispatchQueue.main.asyncAfter(deadline: .now() + timeInterval, execute: closure)
        }
    }
    
    required init(uuid: UUID, name: String, connection: Connection) {
        super.init(uuid: uuid, name: name, connection: connection)
    }
}
