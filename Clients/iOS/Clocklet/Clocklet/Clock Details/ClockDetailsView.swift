//
//  ClockDetailsView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 10/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import Network
import CombineBluetooth


struct ClockDetailsView: View {
    
    @EnvironmentObject var clock: Clock
    @State var showLocationDetails: Bool = false
    @State var animating: Bool = false
    
    var body: some View {
        ScrollView{
            VStack(){
                Image(uiImage:clock.caseImage)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .frame(width: nil, height: 200, alignment: .center)
                switch clock.state {
                case .connected:
                    
                    switch clock.isConfigured {
                        
                    case .unknown:
                        EmptyView()
                        
                    case .notConfigured:
                        Text("Set up your clocklet...")
                        
                    case .configured:
                        if let settingsService = clock.settingsService {
                            ClockSettingsView().environmentObject(settingsService)
                        }
                    }
                    
                    if let networkService = clock.networkService {
                        NavigationLink(destination: NetworkDetailView().environmentObject(networkService)){
                            NetworkSummaryView().environmentObject(NetworkSummaryViewModel(networkService))
                        }.buttonStyle(PlainButtonStyle())
                    }
                    
                    if let locationService = clock.locationService {
                        Group{
                            NavigationLink(destination:
                                            LocationDetailsView()
                                            .environmentObject(LocationDetailsViewModel(locationService: locationService))
                                            .environmentObject(clock),
                                           isActive: $showLocationDetails)
                            {
                                if (locationService.isConfigured == .configured){
                                    LocationSummaryView(showLocationDetails:$showLocationDetails).environmentObject(locationService)
                                } else {
                                        EmptyView()
                                }
                                
                            }.buttonStyle(PlainButtonStyle())
                            if (locationService.isConfigured != .configured){
                                LocationSummaryView(showLocationDetails:$showLocationDetails).environmentObject(locationService)
                            }
                        }
                    }
                    
                    
                    if let technicalService = clock.technicalService,
                       let deviceInfoService = clock.deviceInfoService {
                        NavigationLink(destination:
                                        ClockTechnicalView().environmentObject(technicalService)
                                        .environmentObject(deviceInfoService)){
                            ConfigItemView(icon: Image(systemName:"wrench") ,
                                           title: "Technical stuff", disclosure: true){
                                EmptyView()
                            }
                        }.buttonStyle(PlainButtonStyle())
                    }
                case .connecting:
                    VStack{
                            Image(systemName:clock.state.iconSystemName)
                                .resizable()
                                .aspectRatio(contentMode: .fit)
                                .frame(width: 200, height: 200, alignment: .center)
                                .foregroundColor(clock.state.color)
                                .scaleEffect(animating ? 0.5 : 1)
                                .task{
                                    withAnimation(.linear(duration: 0.5)
                                        .repeatForever(autoreverses: true)) {
                                            animating.toggle()
                                    }
                                    
                                }
                        Text(clock.state.description).bold()
                        Spacer()
                        if let lastErrorDescription = clock.state.lastErrorDescription {
                            Text(lastErrorDescription).lineLimit(nil).fixedSize(horizontal: false, vertical: true)
                        }
                    }.padding().frame(maxWidth: .infinity)

                case .disconnected(let error):
                    if let error = error {
                        Text(error.localizedDescription).lineLimit(nil).fixedSize(horizontal: false, vertical: true)
                        Spacer()
                    }
                    Button("Reconnect") {
                        self.clock.connect()
                    }.buttonStyle(RoundyButtonStyle())
                }

            }
            .padding()
            
            
        }.navigationBarTitle( Text(clock.name), displayMode:.automatic)
        .navigationBarItems(trailing: Image(systemName:clock.state.iconSystemName).foregroundColor(clock.state.color))
        .onAppear {
            self.clock.connect()
        }
    }
}

extension ContentSizeCategory{
    static func allCases() -> [ContentSizeCategory]{
        return self.allCases
    }
}

struct ClockDetailsView_Previews: PreviewProvider {
    static let clock: Clock = {
        let clock = Clock("Foop",.tequilla)
        clock.hwRev = 5
        return clock
    }()
    
    static let connectedClock: Clock = {
        let clock = Clock("Foop",.gold)
        clock.state = .connected
        clock.hwRev = 5
        clock.isConfigured = .configured
        clock.settingsService = SettingsService()

        clock.settingsService?.brightness = 0.5
        clock.settingsService?.autoBrightness = false
        
        clock.settingsService?.availableTimeStyles = ["fish","thingy"]
        clock.settingsService?.timeStyle = "fish"
        
        clock.technicalService = TechnicalService()
        clock.deviceInfoService = DeviceInfoService()
        clock.locationService = LocationService()
        clock.locationService?.isConfigured = .notConfigured
        
        return clock
    }()
    
    static var previews: some View {
        Group{
            NavigationView{
                ClockDetailsView().environmentObject(clock)
            }
            NavigationView{
                ClockDetailsView().environmentObject(connectedClock)
            }
        }

        
    }
    
    
}

extension ConnectionState{
    var iconSystemName: String {
        switch self {
        case .connected: return "bolt.fill"
        case .connecting: return "bolt"
        case .disconnected: return "bolt.slash.fill"
        }
    }
    
    var color: Color {
        switch self {
        case .connected: return .green
        case .connecting: return .orange
        case .disconnected: return .red
        }
    }
}

extension ConnectionState: CustomStringConvertible {
    
    public var description: String {
        switch self {
        case .connected: return "Connected"
        case .connecting: return "Connecting"
        case .disconnected: return "Disconnected"
        }
    }
    
    public var lastErrorDescription: String? {
        switch self {
        case .connected, .connecting:
            return nil
        case .disconnected(let error):
            return error?.localizedDescription
        }
    }
}
