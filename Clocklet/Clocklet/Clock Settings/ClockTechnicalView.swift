//
//  ClockTechnicalView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 19/07/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct ClockTechnicalView: View {
    
    @EnvironmentObject var technicalService: TechnicalService
    @EnvironmentObject var deviceInfoService: DeviceInfoService
    
    var body: some View {
        ScrollView{
        VStack(alignment:.leading){
            Spacer()
            VStack(alignment:.leading){
                deviceInfoService.model.map{ model in
                    HStack{
                        Text("Model number:")
                        Text(model).bold()
                    }
                }
                deviceInfoService.serialNumber.map{ serialNumber in
                    HStack{
                        Text("Serial number:")
                        Text(serialNumber).bold()
                    }
                }
                
                deviceInfoService.firmwareVersion.map{ firmwareVersion in
                    HStack{
                        Text("Firmware version:")
                        Text(firmwareVersion).bold()
                    }
                }

            }
            Spacer()
            
            technicalService.autoUpdates.map{ _ in
                ConfigItemView(icon: Image(systemName:"goforward"), iconColor: .green, title: "Firmware Updates") {
                    Toggle("Update firmware automatically (checks daily or shortly after booting)", isOn: self.technicalService.autoUpdatesSelected)
                }
            }
            
            ConfigItemView(icon: Image(systemName:"flame"), iconColor: .red, title: "Reset Clocklet") {
                VStack(alignment: .leading, spacing:20){
                    Button(action:{
                        self.technicalService.reset = .reboot
                    }){
                        Text("Reboot")
                            .accentColor(Color(.systemBackground))
                            .frame(maxWidth: .infinity)
                            .padding()
                            .background(Capsule().fill(Color.orange))
                    }
                    Text("Clocklet will reboot. The app should reconnect once it has started.").fixedSize(horizontal: false, vertical: true)
                    
                    Spacer()
                    
                    Button(action:{
                        self.technicalService.reset = .factoryReset;
                    }){
                        Text("Factory Reset")
                            .accentColor(Color(.systemBackground))
                            .frame(maxWidth: .infinity)
                            .padding()
                            .background(Capsule().fill(Color.red))
                    }
                    
                    Text("Factory reset will erase everything including bluetooth pairing information. You will have to go into your phone's bluetooth settings afterwards and delete the entry for the Clocklet.").fixedSize(horizontal: false, vertical: true)
                }
            }
            
            technicalService.staging.map{ _ in
                ConfigItemView(icon: Image(systemName:"ant"), iconColor: .blue, title: "Staging mode") {
                    Toggle("Use staging builds when updating. Might brick your Clocklet.", isOn: self.technicalService.stagingSelected)
                }
            }
            
        }.padding()
        }
        .navigationBarTitle( Text("Technical Stuff"), displayMode:.automatic)
    }
}

