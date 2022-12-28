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




struct ClockListView: View {
    @EnvironmentObject var clockList: ClockListViewModel
    @State var blurMainView: Bool = false
    
    @Namespace var clockListNamespace
    
    var body: some View {
        ZStack{
            
            if #available(iOS 16.0, *) {
                NavigationStack{
                    contents
                }
            } else {
                NavigationView{
                    contents
                }
            }
            
            if let bluetoothStatusViewModel = clockList.bluetoothStatusViewModel {
                BluetoothOverlayView(bluetoothStatus: bluetoothStatusViewModel)
                    .edgesIgnoringSafeArea(.all)
                    .onAppear{
                        blurMainView = true
                    }.onDisappear{
                        blurMainView = false
                    }
            }
        }
    }
    
    
    var contents: some View{
        Group{
            if let selectedClock = clockList.selectedClock {
                ZStack{
                    ScrollView{
                        
                        ClockSummaryView()
                            .environmentObject(selectedClock)
                            .matchedGeometryEffect(id: selectedClock.uuid, in: clockListNamespace)
                        ClockDetailsView()
                            .environmentObject(selectedClock)
                            .transition(.move(edge: .bottom))
                        Spacer()
                    }
                    .safeAreaInset(edge: .top, spacing: 0) {
                        Rectangle().foregroundColor(.clear).frame(height:40)
                    }
                    
                    VStack{
                        Image(systemName: "eye")
                            .resizable()
                            .scaledToFit()
                            .frame(width:30,height:30)
                            .matchedGeometryEffect(id: "eye", in: clockListNamespace)
                            .onTapGesture {
                                withAnimation{
                                    clockList.selectedClock = nil
                                }
                            }
                        Spacer()
                    }
                }
            } else {
                ZStack{
                    
                    ScrollView{
                        ForEach(clockList.clocks) { clock in
                            ClockSummaryView()
                                .environmentObject(clock)
                                .matchedGeometryEffect(id: clock.uuid, in: clockListNamespace)
                                .onTapGesture {
                                    withAnimation{
                                        clockList.selectedClock = clock
                                    }
                                }
                                .transition(AnyTransition.opacity.animation(.easeInOut(duration: 1.0)))
                               
                        }
                        
                    }
                    .safeAreaInset(edge: .top, spacing: 0) {
                        Rectangle().foregroundColor(.clear).frame(height:100)
                    }
                    VStack{
                        scanningView
                        Spacer()
                    }
                }
                .onAppear {
                    print("Clocklist onAppear")
                    DispatchQueue.main.asyncAfter(deadline: .now()+0.5){
                        self.clockList.disconnectAllDevices()
                    }
                    self.clockList.startScanning()
                }.onDisappear(){
                    print("Clocklist onDisappear")
                    self.clockList.stopScanning()
                }
                
            }
        }
        .navigationBarHidden(true)
        .blur(radius: blurMainView ? 5.0 : 0)
        
    }
    
    @Environment(\.colorScheme) var colorScheme: ColorScheme
    
    
    var scanningView: some View{
        VStack{
            if clockList.bluetoothState == .poweredOn {
                Image(systemName: "eye" )
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .frame(width: 40 , height: 50, alignment: .center)
                    .scaleEffect(clockList.isScanning ? 2 : 1)
                    .opacity(clockList.isScanning ? 1.0 : 0.5)
                    .animation(.spring(response: 1,dampingFraction: 0.3, blendDuration: 0), value: clockList.isScanning)
                    .matchedGeometryEffect(id: "eye", in: clockListNamespace)
                Group{
                    if clockList.isScanning {
                        Text("Looking for Clocklets")
                            .frame(maxWidth:.infinity)
                    }
                }.transition(.scale)
            }
            Rectangle()
                .frame(maxWidth:.infinity,maxHeight:0)
        }
        .frame(height:120)
        .background(LinearGradient(colors: [Color(UIColor.systemBackground),.clear], startPoint: .top, endPoint: .bottom))
        .onTapGesture {
            self.clockList.toggleScanning()
        }
        .animation(.easeInOut, value: clockList.isScanning)
        
    }
}


struct ClockListView_Previews: PreviewProvider {
    
    
    static let clockList: ClockListViewModel = {
        let clockList = ClockListViewModel(central:nil)
        clockList.bluetoothState = .poweredOn
        clockList.isScanning = false
        clockList.clocks = [
        ]
        
        DispatchQueue.main.asyncAfter(deadline: .now()+1) {
            clockList.isScanning = true
        }
        DispatchQueue.main.asyncAfter(deadline: .now()+2) {
            clockList.clocks.append(Clock("Blackie", .black))
        }
        DispatchQueue.main.asyncAfter(deadline: .now()+4) {
            clockList.clocks.append(Clock("Clockola", .translucent))
        }
        DispatchQueue.main.asyncAfter(deadline: .now()+6) {
            clockList.clocks.append(Clock("Boingy", .bluePink))
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
            
        }
        
        
        
    }
}
