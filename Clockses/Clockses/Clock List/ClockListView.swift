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

struct ClockListView: View {
    @ObservedObject var viewModel: ClockListViewModel = ClockListViewModel()
    
    
    var body: some View {
        NavigationView{
            VStack{
                List(viewModel.clockItems) { clockItem in

                    NavigationLink(destination:
                    ClockDetailsView(viewModel: clockItem.detailsViewModel)){
                        ClockSummaryView(viewModel: clockItem)
                    }.listRowInsets(EdgeInsets(top: 0, leading: 0, bottom: 0, trailing: 40))
                }
                
                if viewModel.isScanning {
                    Text("Scanning").background(Color.green)
                }
            }.navigationBarTitle(Text("Clocks"))
            .onAppear {
                self.viewModel.startScanning()
            }.onDisappear(){
                self.viewModel.stopScanning()
            }
        }
    }

}

struct ClockListView_Previews: PreviewProvider {
    static var previews: some View {
        let listView = ClockListView()
        listView.viewModel.clockItems = [
            ClockSummaryViewModel(clock: Clock("Clock One", .black)),
            ClockSummaryViewModel(clock: Clock("Clock Two", .wood)),
            ClockSummaryViewModel(clock: Clock("Clock Three", .bare))
        ]
        return listView
    }
}
