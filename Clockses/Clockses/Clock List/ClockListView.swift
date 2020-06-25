//
//  ClockListView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 10/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation

import SwiftUI
import Combine
import CombineBluetooth

struct ScanningView: View{
    var scanning: Bool
    
    @State private var animationAmount = 0.3
    
    var body: some View{
        VStack{
            if scanning {
                Image(systemName: "eye").opacity(scanning ? animationAmount : 0.5).animation(
                    Animation.easeInOut(duration: 1)
                    .repeatForever())
                    .onAppear(){
                        self.animationAmount = 0.6
                    }
                    
                Text("Scanning for clocks").opacity(0.5)
                    
                
            } else {
                Image(systemName: "eye.slash").opacity(0.5)
                Text("Not scanning...").opacity(0.5)
            }
        }
        
    }
}

struct ClockListView: View {
    @EnvironmentObject var clockList: ClockList
    
    init(){
        
    }
    
    var body: some View {
        NavigationView{
            VStack{

                    List(clockList.clocks) { clock in
                        NavigationLink(destination:
                                        ClockDetailsView().environmentObject(clock)){
                            ClockSummaryView().environmentObject(clock)
                        }
                        
                    }.background(Color.blue)

                
                    .listRowInsets(EdgeInsets(top: 0, leading: 0, bottom: 0, trailing: 40))
                
                Spacer(minLength: 30)
                
                ScanningView(scanning: clockList.isScanning)
                
                 Spacer(minLength: 30)
            }.navigationBarTitle(Text("Clocks"))

        }.navigationViewStyle(StackNavigationViewStyle())
        .onAppear {
            self.clockList.disconnectAllDevices()
            self.clockList.startScanning()
        }.onDisappear(){
            self.clockList.stopScanning()
        }

    }

}


struct ClockListView_Previews: PreviewProvider {
    

    static let clockList: ClockList = {
        let clockList = ClockList(central:nil)
        clockList.clocks = [
            Clock("One", .black),
            Clock("Two", .blue)
        ]
        return clockList
    }()
    
    
    static var previews: some View {
        Group{
            ClockListView().environmentObject(clockList)
        ScanningView(scanning: true)
        }
        
    }
}
