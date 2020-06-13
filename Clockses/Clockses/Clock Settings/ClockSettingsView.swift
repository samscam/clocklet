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
    @State private var selectedSeparatorAnimation = 0
    
    var body: some View {
        
        VStack{
            
                ConfigItemView(icon: Image(systemName:"rays"), title: "Blink separators") {
                    self.settingsService.availableSeparatorAnimations.map{ available in
                        Picker(selection: self.$selectedSeparatorAnimation, label: Text("")){
                            ForEach(0 ..< available.count) {
                                Text(available[$0])
                            }
                        }.pickerStyle(SegmentedPickerStyle())
                    }

                        
                }
            
        }.padding()
    }
}

struct ClockSettingsView_Previews: PreviewProvider {
    static var previews: some View {
        ClockSettingsView()
    }
}
