//
//  ClockSettingsView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 12/06/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct ClockSettingsView: View {
    
    @EnvironmentObject var settingsService: SettingsService
    
    
    var body: some View {
        ScrollView{
        VStack{
            
                ConfigItemView(icon: Image(systemName:"rays"), title: "Blink separators") {
                    
                    self.settingsService.availableSeparatorAnimations.map{ available in
                        
                        
                        Picker("What do you want", selection: self.$settingsService.separatorAnimationSelection) {
                                
                            ForEach(available, id: \.self ) { item in
                                    Text(item)
                                }
                            }.pickerStyle(SegmentedPickerStyle())
                            
                        }
                        
                        
                }
            Text(self.settingsService.separatorAnimationSelection)
        }.padding()
        }
    }
}

//struct ClockSettingsView_Previews: PreviewProvider {
//    static var previews: some View {
//        ClockSettingsView()
//    }
//}


