//
//  ClockSummaryView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 13/09/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI


struct ClockSummaryView: View {
    
    @EnvironmentObject var clock: Clock
    @Environment(\.colorScheme) var colorScheme: ColorScheme
    
    var body: some View {
        
        ZStack{
            Image(clock.caseColor.imageName).resizable().aspectRatio(contentMode: .fit)
            if colorScheme == .light {
                Rectangle().foregroundColor(.clear).background(LinearGradient(gradient: Gradient(colors:[Color(hue: 0, saturation: 0, brightness: 1, opacity: 0) ,.white]), startPoint: .top, endPoint: .bottom)).padding(.top, 100)
            } else {
                Rectangle().foregroundColor(.clear).background(LinearGradient(gradient: Gradient(colors:[Color(hue: 0, saturation: 0, brightness: 0, opacity: 0) ,.black]), startPoint: .top, endPoint: .bottom)).padding(.top, 100)
            }
            Text(clock.name).font(.largeTitle).bold().shadow(color: colorScheme == .light ? .white : .black, radius: 2, x: 0, y: 0).padding(.top, 100)
        }.frame( minHeight: 200, idealHeight: 200, maxHeight: 200, alignment: .center)
    }
 
}

//struct ClockSummaryView_Previews: PreviewProvider {
//    
//    static var previews: some View {
//        let clock = Clock("Clocklet #002",.wood)
//        clock.caseColor = .wood
//        let vm = ClockSummaryViewModel(clock: clock)
//        return Group {
//            ClockSummaryView(viewModel: vm ).background(Color.black).previewLayout(.sizeThatFits).colorScheme(.dark)
//            ClockSummaryView(viewModel: vm ).previewLayout(.sizeThatFits).colorScheme(.light)
//        }
//    }
//}

//                    VStack {
            //            Image(self.clock.caseColor.imageName).resizable().aspectRatio(contentMode: .fit)
            //            Image("esp32feather")
//                        Text(self.clock.name).font(.largeTitle)
//                        Text("#" + self.clock.serial)
//                    }
