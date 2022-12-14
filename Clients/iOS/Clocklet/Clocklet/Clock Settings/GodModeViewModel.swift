//
//  GodModeViewModel.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 10/12/2022.
//  Copyright © 2022 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import Combine


class GodModeViewModel: ObservableObject {
    
    var godModeService: GodModeService?
    
    var bag = Set<AnyCancellable>()
    
    init(_ godModeService: GodModeService? = nil){

        $maxTmp.sink { [weak self] value in
            DispatchQueue.main.async{
                self?.godModeService?.godModeSettings?.weather.maxTmp = value
            }
        }.store(in: &bag)
        
    }
    
    @Published var isOn: Bool = false
    @Published var minTmp: Float = 0
    @Published var maxTmp: Float = 0
    @Published var precipTypes: [GodModeService.PrecipType] = GodModeService.PrecipType.allCases
    @Published var precipType: GodModeService.PrecipType = .drizzle
    
    @Published var precipIntensity: Float = 0
    
    @Published var lightning: Bool = false
    @Published var rainbows: Bool = false
}
