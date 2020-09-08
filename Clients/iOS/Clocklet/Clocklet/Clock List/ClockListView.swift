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
import CoreBluetooth



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
                    Text("Looking for Clocklets").opacity(0.5)
                } else {
                    Text("Find more Clocklets").opacity(0.5).animation(Animation.easeInOut(duration: 0.5))
                }
            }
        }.onTapGesture {
            self.clockList.toggleScanning()
        }
        
    }
}

struct ClockListView: View {
    @EnvironmentObject var clockList: ClockList
    @State var blurMainView: Bool = false
    
    
    var body: some View {
            ZStack{
                NavigationView{
                    
                    VStack{
                        ScrollView{
                            VStack(spacing:20){
                                Text("Hello! This is the Clocklet app for configuring your Clocklet.")
                                Spacer()
                                VStack{
                                    ForEach(clockList.clocks) { clock in
                                        NavigationLink(destination:
                                                        ClockDetailsView().environmentObject(clock)){
                                            ClockSummaryView()
                                                .environmentObject(clock)
                                        }
                                        .transition(AnyTransition.opacity.animation(.easeInOut(duration: 1.0)))
                                    }}
                                
                                Spacer()
                            }.padding()
                        }
                        
                        ScanningView().padding(.top, 10)
                    }
                    .onAppear {
                        print("Clocklist onAppear")
                        // self.clockList.disconnectAllDevices()
                        self.clockList.startScanning()
                    }.onDisappear(){
                        print("Clocklist onDisappear")
                        self.clockList.stopScanning()
                    }
                    .navigationBarTitle(Text("Clocklet"))
                    
                }
                .navigationViewStyle(StackNavigationViewStyle()).blur(radius: blurMainView ? 5.0 : 0)
                
                if let bluetoothStatusViewModel = clockList.bluetoothStatusViewModel {
                    BluetoothOverlayView(bluetoothStatus: bluetoothStatusViewModel)
                        .edgesIgnoringSafeArea(.all).onAppear{
                            blurMainView = true
                        }.onDisappear{
                            blurMainView = false
                        }
                } else {
                    EmptyView()
                }
            }
        
    }
    
}


struct ClockListView_Previews: PreviewProvider {
    
    
    static let clockList: ClockList = {
        let clockList = ClockList(central:nil)
        clockList.bluetoothState = .unauthorized
        clockList.isScanning = false
        clockList.clocks = [
            Clock("Clocklet #5", .black),
            Clock("Clocklet #6", .bluePink),
            Clock("Clocklet #7", .wood),
            Clock("Clocklet #8", .bones),
        ]
        
        //        DispatchQueue.main.asyncAfter(deadline: .now()+1) {
        //            clockList.isScanning = true
        //        }
        //        DispatchQueue.main.asyncAfter(deadline: .now()+2) {
        //            clockList.clocks.append(Clock("Clocklet #5", .black))
        //        }
        //        DispatchQueue.main.asyncAfter(deadline: .now()+4) {
        //            clockList.clocks.append(Clock("Clocklet #6", .translucent))
        //        }
        //        DispatchQueue.main.asyncAfter(deadline: .now()+6) {
        //            clockList.clocks.append(Clock("Clocklet #6", .bluePink))
        //        }
        //        DispatchQueue.main.asyncAfter(deadline: .now()+7) {
        //            clockList.isScanning = false
        //        }
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
            
        }
        

        
    }
}
