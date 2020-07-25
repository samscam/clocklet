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

extension Clock {
    static func mockClock() -> Clock{
        let mockClock = Clock("Mock Clocklet", .wood)
        mockClock.locationService = LocationService()
        mockClock.locationService?.currentLocation = CurrentLocation(configured: false, lat:  0, lng: 0)
        
        let networkService = NetworkService()
        
        networkService.currentNetwork = CurrentNetwork(status: .disconnected, connected: false, configured: false, ssid: nil, channel: 5, ip: nil, rssi: -10)
        
        networkService.availableNetworks = [
            AvailableNetwork(ssid: "One network", enctype: .wpa2psk, rssi: -30, channel: 5, bssid: "bssone"),
            AvailableNetwork(ssid: "Two network", enctype: .open, rssi: -30, channel: 5, bssid: "bsstwo"),
            AvailableNetwork(ssid: "Three network", enctype: .wpa2psk, rssi: -80, channel: 6, bssid: "bssthree"),
            AvailableNetwork(ssid: "Four network", enctype: .wpa2psk, rssi: -50, channel: 8, bssid: "bssfour"),
            AvailableNetwork(ssid: "Five network", enctype: .wpa2psk, rssi: -20, channel: 5, bssid: "bssfive")
            
        ]
        mockClock.networkService = networkService
        mockClock.state = .connected
        return mockClock
    }
    

}
