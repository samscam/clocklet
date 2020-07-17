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
                        VStack{
                        Picker("What do you want", selection: self.$sepAnim){ // self.$settingsService.separatorAnimation) {
                                    
                                ForEach(available) { item in
                                    Text(item.rawValue)
                                    }
                            }.pickerStyle(SegmentedPickerStyle())
                            

                        Text(self.sepAnim.rawValue)
                        Text(self.settingsService.separatorAnimation?.rawValue ?? "Unknown")
                        }
                    }
                }

            Spacer()
            ConfigItemView(icon: Image(systemName:"flame"), iconColor: .red, title: "Reset Clocklet") {
                VStack(spacing:20){
                    Button("Reboot"){
                        settingsService.reset = .reboot
                    }.accentColor(Color(.systemBackground))
                    .frame(maxWidth: .infinity)
                    .padding()
                    .background(Capsule().fill(Color.orange))
                    .frame(maxWidth: .infinity)
                    Text("Clocklet will reboot. The app should reconnect once it has started.")
                    
                    Spacer()
                    
                    Button("Factory Reset") {
                        settingsService.reset = .factoryReset;
                    }.accentColor(Color(.systemBackground))
                    .frame(maxWidth: .infinity)
                    .padding()
                    .background(Capsule().fill(Color.red))
                    
                    
                    Text("Factory reset will erase everything including bluetooth pairing information. You will have to go into your phone's bluetooth settings afterwards and delete the entry for the Clocklet.")
                }
            }
        }.padding()
        }
        .navigationBarTitle( Text("Technical Stuff"), displayMode:.automatic)
    }
}

//struct ClockSettingsView_Previews: PreviewProvider {
//    static var previews: some View {
//        ClockSettingsView()
//    }
//}


