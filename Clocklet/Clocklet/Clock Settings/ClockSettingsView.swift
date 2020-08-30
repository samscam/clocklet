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
            
            settingsService.brightness.map { _ in
                ConfigItemView(icon: Image(systemName:"sun.max"), title: "Brightness") {
                    VStack{
                        Slider(value: self.settingsService.selectedBrightness, in: 0...1)
                        Toggle("Adjust automatically", isOn: settingsService.b_autoBrightness)
                    }
                }
            }
            settingsService.timeStyle.map { _ in
                ConfigItemView(icon: Image(systemName:"24.circle"), title: "Time Style") {
                    
                    
                    Picker("What do you want", selection: self.settingsService.selectedTimeStyle){
                        ForEach(self.settingsService.timeStyles){ timeStyle in
                                Text(timeStyle)
                        }
                    }.pickerStyle(SegmentedPickerStyle())
                }
            }
//            settingsService.availableSeparatorAnimations.map { _ in
//                ConfigItemView(icon: Image(systemName:"rays"), title: "Blink separators") {
//                    
//                    Picker("What do you want", selection: self.settingsService.selectedAnimation){
//                        ForEach(self.settingsService.separatorAnimations) { item in
//                            Text(item)
//                        }
//                    }.pickerStyle(SegmentedPickerStyle())
//                }
//            }

            
        }
    }
}
