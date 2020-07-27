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
    func mockClockBehaviour() {
        
        // Sequence of events and behaviours for the mock clock

        self.willConnect = {
            self.state = .connecting
            let sequence: [TimeInterval: (()->Void)] =
                [1: { self.state = .connected },
                 1.25: {
                    let networkService = NetworkService()
                    self.networkService = networkService
                    
                    // Simulate ability to join a network
                    networkService.$joinNetwork.sink { (joinNetwork) in
                        if let joinNetwork = joinNetwork {
                            networkService.currentNetwork=CurrentNetwork(status: .idle, connected: false, configured: true, ssid: joinNetwork.ssid, channel: 13, ip: nil, rssi: -30)
                            DispatchQueue.main.asyncAfter(deadline: .now() + 2){
                                if let currentNetwork = networkService.currentNetwork {
                                    let nextNetwork = CurrentNetwork(status: .connected, connected: true, configured: true, ssid: currentNetwork.ssid, channel: currentNetwork.channel, ip: IPv4Address("192.168.0.42"), rssi: -20)
                                    networkService.currentNetwork = nextNetwork
                                }
                                
                            }
                        }
                    }.store(in: &self.bag)
                    networkService.currentNetwork = CurrentNetwork(status: .disconnected, connected: false, configured: false, ssid: nil, channel: 5, ip: nil, rssi: -10)
                    
                    networkService.availableNetworks = [
                        AvailableNetwork(ssid: "One network", enctype: .wpa2psk, rssi: -30, channel: 5, bssid: "bssone"),
                        AvailableNetwork(ssid: "Two network", enctype: .open, rssi: -30, channel: 5, bssid: "bsstwo"),
                        AvailableNetwork(ssid: "Three network", enctype: .wpa2psk, rssi: -80, channel: 6, bssid: "bssthree"),
                        AvailableNetwork(ssid: "Four network", enctype: .wpa2psk, rssi: -50, channel: 8, bssid: "bssfour"),
                        AvailableNetwork(ssid: "Five network", enctype: .wpa2psk, rssi: -20, channel: 5, bssid: "bssfive")
                        
                    ]
                    
                    
                    },
                 1.5:      {
                    self.locationService = LocationService()
                     self.locationService?.currentLocation = CurrentLocation(configured: false, lat:  0, lng: 0)
                    
                    self.settingsService = SettingsService()
                    self.settingsService?.availableTimeStyles = ["24 Hour","12 Hour","Decimal","Wonky"]
                    self.settingsService?.timeStyle = "24 Hour"
                    self.settingsService?.availableSeparatorAnimations = ["Static","Blinky","Fade"]
                    self.settingsService?.separatorAnimation = "Fade"
                    
                    self.technicalService = TechnicalService()
                    self.deviceInfoService = DeviceInfoService()
                    self.deviceInfoService?.firmwareVersion = "1.2.3"
                    self.deviceInfoService?.serialNumber = "54321"
                    self.deviceInfoService?.model = "0"
                    self.deviceInfoService?.manufacturerName = "Test manufacturer"
                    
                    }
            ]
            sequence.forEach { (timeInterval, closure) in
                DispatchQueue.main.asyncAfter(deadline: .now() + timeInterval, execute: closure)
            }
        }
        
    }

}
