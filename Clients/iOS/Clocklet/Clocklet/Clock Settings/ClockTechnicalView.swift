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
    
    @State private var showingResetAlert = false
    
    var body: some View {
        ScrollView{
        VStack(alignment:.leading){
            Spacer()
            
            VStack(alignment:.leading){
                if let model = deviceInfoService.model {
                    HStack{
                        Text("Model number:")
                        Text(model).bold()
                    }
                }
                
                if let serialNumber = deviceInfoService.serialNumber {
                    HStack{
                        Text("Serial number:")
                        Text(serialNumber).bold()
                    }
                }
                
                if let firmwareVersion = deviceInfoService.firmwareVersion {
                    HStack{
                        Text("Firmware version:")
                        Text(firmwareVersion).bold()
                    }
                }
                
                if let versionString = Bundle.main.infoDictionary?["CFBundleShortVersionString"] as? String,
                   let buildNumber = Bundle.main.infoDictionary?["CFBundleVersion"] as? String {
                    HStack{
                        Text("App version:")
                        Text("\(versionString) (\(buildNumber))").bold()
                    }
                }
            }
            Spacer()
            

            
            ConfigItemView(icon: Image(systemName:"flame"), iconColor: .red, title: "Reset Clocklet") {
                VStack(alignment: .leading, spacing:20){
                    Button(action:{
                        self.technicalService.reset = .reboot
                    }){
                        Text("Reboot")
                            .accentColor(Color(.systemBackground))
                    }.buttonStyle(RoundyButtonStyle()).accentColor(.orange)
                    Text("Clocklet will reboot. The app should reconnect once it has started.").fixedSize(horizontal: false, vertical: true)
                    
                    Spacer()
                    
                    Button(action:{
                        self.showingResetAlert = true
                    }){
                        Text("Factory Reset")
                            .accentColor(Color(.systemBackground))
                            
                    }.alert(isPresented: $showingResetAlert) { () -> Alert in
                        Alert(title: Text("Are you sure you want to do a factory reset?"),
                              primaryButton: Alert.Button.destructive(Text("Yes. Nuke from orbit!")){
                                                self.technicalService.reset = .factoryReset
                                            },
                              secondaryButton: Alert.Button.cancel())
                    }.buttonStyle(RoundyButtonStyle()).accentColor(.red)
                    
                    Text("Factory reset will erase everything including bluetooth pairing information. You will have to go into your phone's bluetooth settings afterwards and delete the entry for the Clocklet.").fixedSize(horizontal: false, vertical: true)
                }
            }
            
            technicalService.autoUpdates.map{ _ in
                ConfigItemView(icon: Image(systemName:"goforward"), iconColor: .green, title: "Firmware Updates") {
                    Toggle("Update firmware automatically (checks daily or shortly after booting)", isOn: $technicalService.autoUpdates ?? true)
                }
            }
            inDebugBuilds {
                technicalService.staging.map{ _ in
                    ConfigItemView(icon: Image(systemName:"ant"), iconColor: .blue, title: "Staging mode") {
                        Toggle("Use staging builds when updating. Might brick your Clocklet.", isOn: $technicalService.staging ?? false)
                    }
                }
            }

            
        }.padding()
        }
        .navigationBarTitle( Text("Technical Stuff"), displayMode:.automatic)
    }
}


struct ClockTechnicalView_Previews: PreviewProvider {
    
    static let technicalService: TechnicalService = {
        let technicalService = TechnicalService()
        technicalService.autoUpdates = false
        return technicalService
    }()
    
    static let deviceInfoService: DeviceInfoService = {
        let deviceInfo = DeviceInfoService()
        deviceInfo.firmwareVersion = "1.2.3"
        deviceInfo.serialNumber = "42"
        deviceInfo.model = "12345"
        return deviceInfo
    }()
    
    static var previews: some View {
        ClockTechnicalView()
            .environmentObject(technicalService)
            .environmentObject(deviceInfoService)
        
    }
}
