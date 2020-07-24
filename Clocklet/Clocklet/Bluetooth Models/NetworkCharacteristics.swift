//
//  Network.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 26/09/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import Network
import CoreBluetooth
import CombineBluetooth

protocol Network{
    var channel: Int { get }
    var rssi: Int { get }
}

struct AvailableNetwork: Network, Codable, Identifiable, JSONCharacteristic {
    var id: String { return bssid }
    let ssid: String
    let enctype: AuthMode
    let rssi: Int
    let channel: Int
    let bssid: String
}


struct CurrentNetwork: Network, Codable, JSONCharacteristic {
    let status: WifiStatus
    let connected: Bool
    let configured: Bool
    let ssid: String?
    let channel: Int
    let ip: IPv4Address?
    let rssi: Int
}

struct JoinNetwork: Codable, JSONCharacteristic {
    let ssid: String
    let psk: String?
    let enctype: AuthMode
}

enum WifiStatus: Int, Codable, CaseIterable, Identifiable {
    var id: Int {
        return self.rawValue
    }
    
    case idle = 0
    case noSSID
    case scanCompleted
    case connected
    case connectionFailed
    case connectionLost
    case disconnected
    case noShield = 255
}

extension IPv4Address: Decodable {
    
    enum IpCodingError: Error {
        case invalidAddress
    }
    
    public init(from decoder: Decoder) throws {
        let container = try decoder.singleValueContainer()
        let stringValue = try container.decode(String.self)
        if IPv4Address(stringValue) != nil {
            self.init(stringValue)!
        } else {
            throw(IpCodingError.invalidAddress)
        }
    }
}

extension IPv4Address: Encodable {
    public func encode(to encoder: Encoder) throws {
        var container = encoder.singleValueContainer()
        try container.encode(self.debugDescription)
    }
}

enum AuthMode: Int, Codable, CustomStringConvertible {

    var description: String {
        switch self {
        case .open: return "Open"
        case .wep: return "WEP"
        case .wpapsk: return "WPA"
        case .wpa2psk: return "WPA2"
        case .wpawpa2psk: return "WPA / WPA2"
        case .wpa2enterprise: return "WPA2 Enterprise"
        }
    }
    
    case open = 0
    case wep
    case wpapsk
    case wpa2psk
    case wpawpa2psk
    case wpa2enterprise
}
