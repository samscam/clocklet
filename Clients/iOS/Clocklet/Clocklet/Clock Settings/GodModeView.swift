//
//  GodModeView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 10/12/2022.
//  Copyright © 2022 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct GodModeView: View {
    @EnvironmentObject var godModeService: GodModeService
    
    
    var body: some View {
        ScrollView{
            VStack{
                Text("GOD MODE").font(.largeTitle)
                
                
                Toggle(isOn: $godModeService.enabled ){
                    EmptyView()
                }
                .labelsHidden()
                .scaleEffect(x:2.5,y:2.5)
                .frame(minHeight:50)
                
                temperatureControls
                
                precipitationControls
                

            }
        }
    }
    
    
    
    var temperatureControls: some View{
        
        Group{
            Spacer(minLength: 50)
            Text("Temperature").bold()
            HStack{
                Text("Min").bold()
                Slider(value: $godModeService.minTmp,
                       in: -20...40,
                       step: 5
                ) {
                    Text("Min Temp")
                } minimumValueLabel: {
                    Text("-20")
                } maximumValueLabel: {
                    Text("40")
                }
            }
            
            HStack{
                Text("Max").bold()
                Slider(value: $godModeService.maxTmp,
                       in: -20...40,
                       step: 5
                ) {
                    Text("Min Temp")
                } minimumValueLabel: {
                    Text("-20")
                } maximumValueLabel: {
                    Text("40")
                }
            }
            Spacer()
        }
    }
    
    
    var precipitationControls: some View {
        
        Group{
            Divider()
            Spacer()
            
            Text("Precipitation").bold()
            
            Picker(selection: godMode.weather.precipType) {
                ForEach(GodModeService.PrecipType.allCases, id: \.self){ val in
                    Text(val.description)
                }
            } label: {
                EmptyView()
            }.pickerStyle(.segmented)
            HStack{
                Text("Intensity (mm/h)").bold()
                Slider(value: godMode.precipIntensity,
                       in: 0...80
                ) {
                    Text("Precipitation Intensity")
                } minimumValueLabel: {
                    Text("0")
                } maximumValueLabel: {
                    Text("80")
                }
            }
        }
        
    }
    
    var effects: some View {
        Group{
            Divider()
            Spacer()
            
            Text("Effects").bold()
            Toggle(isOn: godMode.thunder){Text("⚡️ Lightning")}
                .fixedSize()
            Toggle(isOn: godMode.rainbows){Text("🌈 Rainbows")}
                .fixedSize()
        }
    }
}

struct GodModeView_Previews: PreviewProvider {
    
    static let godModeViewModel = GodModeViewModel( GodModeService())
    static let clock = Clock("Woop")
    
    static var previews: some View {
        GodModeView().environmentObject(clock)
    }
}

