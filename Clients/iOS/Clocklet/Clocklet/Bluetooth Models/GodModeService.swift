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

class GodModeService: ServiceProtocol {
    
    var bag = Set<AnyCancellable>()
    
    required init(){
        godModeSettings = GodModeSettings()
        
        $enabled.sink { [weak self] newVal in
            self?.godModeSettings?.enabled = newVal
        }.store(in: &bag)
        
        $minTmp.sink { [weak self] newVal in
            self?.godModeSettings?.weather.minTmp = Float(newVal)
        }.store(in: &bag)
        
        $maxTmp.sink { [weak self] newVal in
            self?.godModeSettings?.weather.maxTmp = Float(newVal)
        }.store(in: &bag)
    }
    
    @Characteristic("603B79B2-568A-4AFF-8B9A-08DF689C9D49") var godModeSettings: GodModeSettings?
    
    
    @Published var enabled: Bool = false
    @Published var minTmp: Double = 0
    @Published var maxTmp: Double = 0
    
    struct GodModeSettings: Codable, JSONCharacteristic {
        var enabled: Bool = false
        var weather: Weather = Weather()
    }

    struct Weather: Codable {
        var type: Int = 0
        var precipChance: Float = 0
        var precipIntensity: Float = 0
        var precipType: PrecipType = .rain
        var maxTmp: Float = 0
        var minTmp: Float = 0
        var currentTmp: Float = 0
        var thunder: Bool = false
        var windSpeed: Float = 0
        var cloudCover: Float = 0
        var pressure: Float = 0
        var rainbows: Bool = false
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

