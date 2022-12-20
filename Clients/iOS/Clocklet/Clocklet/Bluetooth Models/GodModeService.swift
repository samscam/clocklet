//
//  GodModeService.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 10/12/2022.
//  Copyright © 2022 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CombineBluetooth
import Combine
import SwiftUI

class GodModeService: ServiceProtocol {
    
    var bag = Set<AnyCancellable>()
    
    required init(){}
    
    @Characteristic("603B79B2-568A-4AFF-8B9A-08DF689C9D49") var enabled: Bool?
    @Characteristic("CAA822A1-4CFB-4F7B-8169-3EB56DEADA13") var weather: Weather?
    

    struct Weather: Codable, JSONCharacteristic {
        var precipChance: Float
        var precipIntensity: Float
        var precipType: PrecipType
        var maxTmp: Float
        var minTmp: Float
        var windSpeed: Float
        var thunder: Bool
        var rainbows: Bool
    }

    enum PrecipType: Int, Codable, CaseIterable, Identifiable {
        
        var id: Int { return self.rawValue }
        
        case drizzle = 0
        case rain = 1
        case sleet = 2
        case snow = 3
        
        var description: String {
            switch self {
            case .drizzle: return "Drizzle"
            case .rain: return "Rain"
            case .sleet: return "Sleet"
            case .snow: return "Snow"
            }
        }
    }

}

extension Float {
    var beaufortDescription: String{
        switch self {
        case 0..<0.5:
            return "Calm"
        case 0.5..<1.5:
            return "Light air"
        case 1.5..<3.3:
            return "Light breeze"
        case 3.3..<5.5:
            return "Gentle breeze"
        case 5.5..<8:
            return "Moderate breeze"
        case 8..<10.7:
            return "Fresh breeze"
        case 10.7..<13.8:
            return "Strong breeze"
        case 13.8..<17.1:
            return "High wind"
        case 17.1..<20.7:
            return "Gale"
        case 20.7..<24.4:
            return "Severe gale"
        case 24.4..<28.4:
            return "Storm"
        case 28.4..<32.6:
            return "Violent storm"
        case 32.6...:
            return "Hurricane"
        default:
            return "BONKERS"
        }
    }
    var beaufortNumber: Int{
        switch self {
        case 0..<0.5:
            return 0
        case 0.5..<1.5:
            return 1
        case 1.5..<3.3:
            return 2
        case 3.3..<5.5:
            return 3
        case 5.5..<8:
            return 4
        case 8..<10.7:
            return 5
        case 10.7..<13.8:
            return 6
        case 13.8..<17.1:
            return 7
        case 17.1..<20.7:
            return 8
        case 20.7..<24.4:
            return 9
        case 24.4..<28.4:
            return 10
        case 28.4..<32.6:
            return 11
        case 32.6...:
            return 12
        default:
            return 13
        }
    }
}
