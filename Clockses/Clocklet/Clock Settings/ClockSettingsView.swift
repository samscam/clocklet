//
//  ClockSettingsView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 12/06/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct ClockSettingsView: View {
    
    @EnvironmentObject var settingsService: SettingsService
    
    @State var sepAnim: SeparatorAnimation = .Static
    
    var body: some View {
        ScrollView{
        VStack{
            
                ConfigItemView(icon: Image(systemName:"rays"), title: "Blink separators") {
                    
                    self.settingsService.availableSeparatorAnimations.map{ available in
                        
                        Picker("What do you want", selection: self.$sepAnim){ // self.$settingsService.separatorAnimation) {
                                    
                                ForEach(available) { item in
                                    Text(item.rawValue)
                                    }
                            }.pickerStyle(SegmentedPickerStyle())
                            
                        }

                        
                        
                }
            Text(self.sepAnim.rawValue)
            Text(self.settingsService.separatorAnimation?.rawValue ?? "Unknown")
        }.padding()
        }
    }
}

//struct ClockSettingsView_Previews: PreviewProvider {
//    static var previews: some View {
//        ClockSettingsView()
//    }
//}


