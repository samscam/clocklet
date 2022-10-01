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
        VStack{
            Spacer()
            ZStack{
                Image(uiImage:clock.caseImage).renderingMode(.original).resizable().aspectRatio(contentMode: .fit)
                
                if colorScheme == .light {
                    Rectangle().foregroundColor(.clear).background(
                        LinearGradient(gradient: Gradient(colors:[Color(hue: 0, saturation: 0, brightness: 1, opacity: 0) ,.white]), startPoint: .top, endPoint: .bottom)
                            )
                        .padding(.top, 100)
                } else {
                    Rectangle().foregroundColor(.clear).background(LinearGradient(gradient: Gradient(colors:[Color(hue: 0, saturation: 0, brightness: 0, opacity: 0) ,.black]), startPoint: .top, endPoint: .bottom)).padding(.top, 100)
                }
                Text(clock.name).font(.largeTitle).bold()
                    .foregroundColor(.primary)
                    .shadow(color: colorScheme == .light ? .white : .black, radius: 2, x: 0, y: 0).padding(.top, 100)
            }
            Spacer()
        }

    }
 
}

struct ClockSummaryView_Previews: PreviewProvider {
    static let clock = Clock("One", .black)
    
    
    static var previews: some View {
        ClockSummaryView()
            .environmentObject(clock)
        
    }
}
