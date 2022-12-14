//
//  ClockSettingsView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 12/06/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

extension String: Identifiable {
    
    public var id: String {

        return self
    }
}

struct ClockSettingsView: View {
    
    @EnvironmentObject var settingsService: SettingsService
    
    
    var body: some View {

        
        VStack{
            if settingsService.brightness != nil && settingsService.autoBrightness != nil {
                ConfigItemView(icon: Image(systemName:"sun.max"), title: "Brightness") {
                    VStack{
                        Slider(value: $settingsService.brightness ?? 0.5, in: 0...1)
                        Toggle("Adjust automatically", isOn: $settingsService.autoBrightness ?? false)
                    }
                }
            }
            
            if settingsService.timeStyle != nil {
                ConfigItemView(icon: Image(systemName:"24.circle"), title: "Time Style") {
                    
                    
                    Picker("Time Style", selection: $settingsService.timeStyle ?? "24 Hour"){
                        ForEach(self.settingsService.timeStyles){ timeStyle in
                                Text(timeStyle)
                        }
                    }.pickerStyle(SegmentedPickerStyle())
                }
            }
            
//            settingsService.availableSeparatorAnimations.map { _ in
//                ConfigItemView(icon: Image(systemName:"rays"), title: "Blink separators") {
//                    
//                    Picker("What do you want", selection:
//                        settingsService.b_separatorAnimation){
//                        ForEach(self.settingsService.separatorAnimations) { item in
//                            Text(item)
//                        }
//                    }.pickerStyle(SegmentedPickerStyle())
//                }
//            }

            
        }
    }
}

