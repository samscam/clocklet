//
//  ClockSummaryView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 13/09/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI


struct ClockSummaryView: View {
    
    @State var clock: Clock
    
    var body: some View {
        VStack(alignment: .center, spacing: -10){
            Text(self.clock.name).font(.largeTitle)
            Image(self.clock.caseColour.imageName).resizable().aspectRatio(contentMode: .fit)
            
        } 
    }
 
}

struct ClockSummaryView_Previews: PreviewProvider {
    static var previews: some View {
        ClockSummaryView(clock: Clock(uuid: UUID(), name: "Some nice clock"))
    }
}

//                    VStack {
            //            Image(self.clock.caseColour.imageName).resizable().aspectRatio(contentMode: .fit)
            //            Image("esp32feather")
//                        Text(self.clock.name).font(.largeTitle)
//                        Text("#" + self.clock.serial)
//                    }
