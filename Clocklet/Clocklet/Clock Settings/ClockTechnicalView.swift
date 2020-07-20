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
        VStack{
            VStack{
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
            
            ConfigItemView(icon: Image(systemName:"flame"), iconColor: .red, title: "Reset Clocklet") {
                VStack(spacing:20){
                    Button(action:{
                        technicalService.reset = .reboot
                    }){
                        Text("Reboot")
                            .accentColor(Color(.systemBackground))
                            .frame(maxWidth: .infinity)
                            .padding()
                            .background(Capsule().fill(Color.orange))
                    }
                    Text("Clocklet will reboot. The app should reconnect once it has started.")
                    
                    Spacer()
                    
                    Button(action:{
                        technicalService.reset = .factoryReset;
                    }){
                        Text("Factory Reset")
                            .accentColor(Color(.systemBackground))
                            .frame(maxWidth: .infinity)
                            .padding()
                            .background(Capsule().fill(Color.red))
                    }
                    
                    Text("Factory reset will erase everything including bluetooth pairing information. You will have to go into your phone's bluetooth settings afterwards and delete the entry for the Clocklet.")
                }
            }
        }.padding()
        }
        .navigationBarTitle( Text("Technical Stuff"), displayMode:.automatic)
    }
}

