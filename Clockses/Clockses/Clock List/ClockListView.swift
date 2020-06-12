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

struct ClockListView: View {
    @EnvironmentObject var clockList: ClockList
    
    var body: some View {
        NavigationView{
            VStack{
                List(clockList.clocks) { clock in

                    NavigationLink(destination:
                    ClockDetailsView().environmentObject(clock)){
                        ClockSummaryView().environmentObject(clock)
                    }.listRowInsets(EdgeInsets(top: 0, leading: 0, bottom: 0, trailing: 40))
                }
                
                if clockList.isScanning {
                    Text("Scanning").background(Color.green)
                }
            }.navigationBarTitle(Text("Clocks"))
            .onAppear {
                self.clockList.disconnectAllDevices()
                self.clockList.startScanning()
            }.onDisappear(){
                self.clockList.stopScanning()
            }
        }
    }

}

//struct ClockListView_Previews: PreviewProvider {
//    static var previews: some View {
//        let listView = ClockListView()
//        listView.viewModel.clockItems = [
//            ClockSummaryViewModel(clock: Clock("Clock One", .black)),
//            ClockSummaryViewModel(clock: Clock("Clock Two", .wood)),
//            ClockSummaryViewModel(clock: Clock("Clock Three", .bones))
//        ]
//        return listView
//    }
//}
