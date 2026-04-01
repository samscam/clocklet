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
    @EnvironmentObject var central: Central
    @StateObject var viewModel = ClockListViewModel()
    
    @Namespace var clockListNamespace
    
    var body: some View {
        
        NavigationStack{
           
            ScrollView{
                
                VStack(alignment:.center) {
                    if viewModel.isScanning {
                        Text("Looking for Clocklets")
                            .frame(maxWidth:.infinity)
                            .transition(.scale)
                            .animation(.easeInOut, value: viewModel.isScanning)
                    }
                    ForEach(viewModel.clocks) { clock in
                        NavigationLink(value:clock){
                            ClockSummaryView()
                                .environmentObject(clock)
                                .matchedTransitionSource(id: clock.uuid, in: clockListNamespace)
                        }.transition(.opacity)
                    }
                    
                }
                .padding(.horizontal)
            }
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .principal) {
                    scanningView
                }
            }
            .navigationDestination(for: Clock.self) { clock in
                ClockDetailsView()
                    .navigationTransition(.zoom(sourceID: clock.uuid, in: clockListNamespace))
                    .environmentObject(clock)
            }
            
        }

        .onAppear{
            print("Clocklist onAppear")
            viewModel.central = central
            
                Task{
                    viewModel.disconnectAllDevices()
                }
            withAnimation {
                viewModel.startScanning()
            }
            
        }
        .onDisappear(){
            print("Clocklist onDisappear")
            self.viewModel.stopScanning()
        }
        
    }
    
    
    
    
    var scanningView: some View{
        VStack{
            
            Image(systemName: "eye" )
                .resizable()
                .aspectRatio(contentMode: .fit)
                .scaleEffect(viewModel.isScanning ? 2 : 1)
                .opacity(viewModel.isScanning ? 1.0 : 0.5)
                .animation(.spring(response: 1,dampingFraction: 0.3, blendDuration: 0), value: viewModel.isScanning)
        }
        .onTapGesture {
            withAnimation {
                self.viewModel.toggleScanning()
            }
            
        }

        
    }
}


#Preview {
    let viewModel = {
        let viewModel = ClockListViewModel()
        
        viewModel.showBluetoothOverlay = false
        viewModel.isScanning = false
        viewModel.clocks = [
        ]
        DispatchQueue.main.asyncAfter(deadline: .now()+1) {
            viewModel.isScanning = true
        }
        DispatchQueue.main.asyncAfter(deadline: .now()+2) {
            viewModel.clocks.append(Clock("Blackie", .black))
        }
        DispatchQueue.main.asyncAfter(deadline: .now()+4) {
            viewModel.clocks.append(Clock("Clockola", .translucent))
        }
        DispatchQueue.main.asyncAfter(deadline: .now()+6) {
            viewModel.clocks.append(Clock("Boingy", .bluePink))
        }
        DispatchQueue.main.asyncAfter(deadline: .now()+7) {
            viewModel.isScanning = false
        }
        return viewModel
    }()
    
    ClockListView().environmentObject(viewModel)
}
