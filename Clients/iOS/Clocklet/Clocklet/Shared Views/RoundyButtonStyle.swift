//
//  RoundyButtonStyle.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 25/07/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI


struct RoundyButtonStyle: ButtonStyle {

    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .foregroundColor(.white)
            .padding()
            .background(Capsule().fill(Color.accentColor))
        
    }
    
}

extension Text {
    func lozenge() -> some View{
        return self.foregroundColor(.white)
        .padding()
        .background(Capsule().fill(Color.accentColor))
    }
}
