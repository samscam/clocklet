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
                List(viewModel.clockConnections) { clockConnection in

                    NavigationLink(destination:
                    ClockDetailsView(clockConnection: clockConnection)){
                        ClockSummaryView(clock: clockConnection.clock)
                    }
                }
                
                if viewModel.isScanning {
                    Text("Scanning").background(Color.green)
                }
            }.navigationBarTitle(Text("Clocklets"))
        }.onAppear {
            self.viewModel.startScanning()
        }
    }

}

struct ClockListView_Previews: PreviewProvider {
    static var previews: some View {
        ClockListView()
    }
}
