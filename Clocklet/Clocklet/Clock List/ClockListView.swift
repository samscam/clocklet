//
//  ClockListView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 10/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation

import SwiftUI
import Combine
import CombineBluetooth

struct ScanningView: View{
    @EnvironmentObject var clockList: ClockList
    @Environment(\.colorScheme) var colorScheme: ColorScheme
    
    
    var body: some View{
        VStack{
            if clockList.bluetoothState == .poweredOn {
            Image(systemName: self.clockList.isScanning ? "eye" : "eye.slash")
                .resizable()
                .aspectRatio(contentMode: .fit)
                .frame(width: 40 , height: 50, alignment: .center)
                .scaleEffect(self.clockList.isScanning ? 2 : 1)
                .opacity(self.clockList.isScanning ? 1.0 : 0.5)
                .animation(Animation.spring())
            
            if self.clockList.isScanning {
                Text("Scanning for Clocklets").opacity(0.5)
            } else {
                Text("Tap to scan").opacity(0.5).animation(Animation.easeInOut(duration: 0.5))
            }
            } else {
                Text("Bluetooth state: \(clockList.bluetoothState.rawValue)")
            }
        }.onTapGesture {
            self.clockList.toggleScanning()
        }
//        .frame( maxWidth: .infinity)
        //        .background(LinearGradient(gradient: Gradient(colors: [Color.white.opacity(0),.white]), startPoint: .top, endPoint: .bottom))
        
    }
}

struct ClockListView: View {
    @EnvironmentObject var clockList: ClockList
    @State var hasAlreadyAppeared = false
    
    init(){
        
    }
    
    var body: some View {
        
        NavigationView{
            VStack{
                    ScrollView{
                        VStack(spacing:20){
                        Text("Hello! This is the Clocklet app for configuring your Clocklet.")
                        Spacer()
                        if clockList.clocks.count == 0 {
                            Text("I've not got a clocklet").bold().foregroundColor(Color(.systemBackground)).frame(maxWidth: .infinity)
                                    

                            .transition(AnyTransition.opacity.animation(.easeInOut(duration: 1.0)))
                            .padding()
                            .background(Capsule().fill(Color.accentColor))
                            .onTapGesture {
                                if let url = URL(string:"http://clocklet.co.uk") {
                                    UIApplication.shared.open(url)
                                }
                                
                            }
                            .onLongPressGesture(minimumDuration: 5, maximumDistance: 30, perform: {
                                self.clockList.createFakeClock()
                            })

                                
                        }
                            
                        ForEach(clockList.clocks) { clock in
                            NavigationLink(destination:
                                            ClockDetailsView().environmentObject(clock)){
                                ClockSummaryView()
                                    .environmentObject(clock)
                            }
                            .transition(AnyTransition.opacity.animation(.easeInOut(duration: 1.0)))

                            
                        }
                        Spacer()
                        }.padding()
                    }
                    
                    ScanningView()
                        .padding(.top, 10)
                    
                
            }
            .navigationBarTitle(Text("Clocklet"))
            .onAppear {
                // This should go back in if/when the bug is fixed
//                self.clockList.disconnectAllDevices()
                self.clockList.startScanning()
            }.onDisappear(){
                self.clockList.stopScanning()
                
            }
        }
        
        
        .navigationViewStyle(StackNavigationViewStyle())
        
        
        
    }
    
}


struct ClockListView_Previews: PreviewProvider {
    
    
    static let clockList: ClockList = {
        let clockList = ClockList(central:nil)
        
        clockList.clocks = [
//            Clock("Clocklet #5", .black),
//            Clock("Clocklet #6", .blue),
//            Clock("Clocklet #7", .wood),
//            Clock("Clocklet #8", .bones),
        ]
        
        DispatchQueue.main.asyncAfter(deadline: .now()+1) {
            clockList.isScanning = true
        }
        DispatchQueue.main.asyncAfter(deadline: .now()+2) {
            clockList.clocks.append(Clock("Clocklet #5", .black))
        }
        DispatchQueue.main.asyncAfter(deadline: .now()+4) {
            clockList.clocks.append(Clock("Clocklet #6", .translucent))
        }
        DispatchQueue.main.asyncAfter(deadline: .now()+6) {
            clockList.clocks.append(Clock("Clocklet #6", .bluePink))
        }
        DispatchQueue.main.asyncAfter(deadline: .now()+7) {
            clockList.isScanning = false
        }
        return clockList
    }()
    
    
    init(){

        
    }
    
    static var previews: some View {
        Group{
            ClockListView().environmentObject(clockList)
                .preferredColorScheme(.dark)
            ClockListView().environmentObject(clockList)
                .preferredColorScheme(.light)
            ScanningView().environmentObject(clockList)
                .preferredColorScheme(.light)
                .previewLayout(.sizeThatFits)
            ScanningView()
                .preferredColorScheme(.dark)
                .environmentObject(clockList)
                
                .previewLayout(.sizeThatFits)
        }
        
        
    }
}

