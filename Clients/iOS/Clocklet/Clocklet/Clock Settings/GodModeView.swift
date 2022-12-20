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
                VStack(alignment:.leading){
                    HStack{
                        Image(systemName: "hand.point.right")
                            .resizable()
                            .scaledToFit()
                            .foregroundColor(Color("Black-ish"))
                            .frame(maxWidth: 100,maxHeight: 100)
                    
                        Toggle(isOn: $godModeService.enabled ?? false){
                            Text("Enabled")
                        }
                        .labelsHidden()
                        .scaleEffect(x:2.5,y:2.5)
                        .frame(minWidth: 140, minHeight: 85)
                    }

//                    .background(.blue)
                    
                    Spacer(minLength: 50)
                    
                    if godModeService.weather == nil {
                        ErrorView("There was no weather data from the clocklet")
                    } else {
                        if (godModeService.enabled ?? false){
                            temperatureControls
                            precipitationControls
                            windControls
                            effects
                        }
                    }
                    
                    // Rectangle to ensure that the scrollview is pushed to the margins
                    Rectangle()
                        .frame(maxWidth:.infinity,maxHeight:0)

                }
                .padding(20)
            }
            .navigationTitle("GOD MODE")

    }
    


    var temperatureControls: some View{
        let minTmp = Binding (
            get: { return godModeService.weather?.minTmp ?? 0.0 },
            set: { godModeService.weather?.minTmp = $0 }
        )
        
        let maxTmp = Binding (
            get: { return godModeService.weather?.maxTmp ?? 0.0 },
            set: { godModeService.weather?.maxTmp = $0 }
        )
        return Group{
            Text("Temperature").bold()
            HStack{
                Text("Min").bold()
                Slider(value: minTmp,
                       in: -20...40,step: 1
                ) {
                    Text("Min Temp")
                } minimumValueLabel: {
                    Text("-20")
                } maximumValueLabel: {
                    Text("40")
                }
                Text("\(Int(minTmp.wrappedValue))")
                    .frame(width:50)
                    .lozenge()
                    
            }
            
            HStack{
                Text("Max").bold()
                Slider(value: maxTmp,
                       in: -20...40,
                       step: 1
                ) {
                    Text("Min Temp")
                } minimumValueLabel: {
                    Text("-20")
                } maximumValueLabel: {
                    Text("40")
                }
                Text("\(Int(maxTmp.wrappedValue))")
                    .bold()
                    .frame(width:50)
                    .lozenge().accentColor(.red)
                    
            }
            Spacer()
        }
    }
    
    
    var precipitationControls: some View {
        
        let precipType = Binding (
            get: { return godModeService.weather?.precipType ?? .rain },
            set: { godModeService.weather?.precipType = $0 }
        )
        
        let precipIntensity = Binding (
            get: { return godModeService.weather?.precipIntensity ?? 0.0 },
            set: { godModeService.weather?.precipIntensity = $0 }
        )
        
        return Group{
            Divider()
            Spacer()
            
            Text("Precipitation").bold()
            
            Picker(selection: precipType) {
                ForEach(GodModeService.PrecipType.allCases, id: \.self){ val in
                    Text(val.description)
                }
            } label: {
                EmptyView()
            }.pickerStyle(.segmented)
            Text("Intensity (mm/h)").bold()
            HStack{
                Slider(value: precipIntensity,
                       in: 0...50,step:1
                ) {
                    Text("Precipitation Intensity")
                } minimumValueLabel: {
                    Text("0")
                } maximumValueLabel: {
                    Text("80")
                }
                Text("\(Int(precipIntensity.wrappedValue))")
                    .bold()
                    .frame(width:50)
                    .lozenge().accentColor(.cyan)
                    
            }
        }
        
    }
    var windControls: some View {
        
        let windSpeed = Binding (
            get: { return godModeService.weather?.windSpeed ?? 0.0 },
            set: { godModeService.weather?.windSpeed = $0 }
        )
        
        return Group{
            Divider()
            Spacer()
            
            Text("Wind Speed (m/s)").bold()
            
            Text(windSpeed.wrappedValue.beaufortDescription).lozenge().accentColor(.green)
            
            HStack{
                Slider(value: windSpeed,
                       in: 0...35,step:1
                ) {
                    Text("Wind speed")
                } minimumValueLabel: {
                    Text("0")
                } maximumValueLabel: {
                    Text("35")
                }
                Text("\(Int(windSpeed.wrappedValue))")
                    .bold()
                    .frame(width:50)
                    .lozenge().accentColor(.green)
            }

        }
        
    }
    
    var effects: some View {
        let thunder = Binding (
            get: { return godModeService.weather?.thunder ?? false },
            set: { godModeService.weather?.thunder = $0 }
        )
        let rainbows = Binding (
            get: { return godModeService.weather?.rainbows ?? false },
            set: { godModeService.weather?.rainbows = $0 }
        )
        
        return Group{
            Divider()
            Spacer()
            
            Text("Effects").bold()
            Toggle(isOn: thunder){Text("⚡️ Thunder")}
                .fixedSize()
            Toggle(isOn: rainbows){Text("🌈 Rainbows")}
                .fixedSize()
        }
    }
}

struct GodModeView_Previews: PreviewProvider {
    
    static var godModeService: GodModeService {
        let gm = GodModeService()
        gm.enabled = true
        
        return gm
    }
    static var previews: some View {
        NavigationView {
            GodModeView().environmentObject(godModeService)
        }
    }
}

