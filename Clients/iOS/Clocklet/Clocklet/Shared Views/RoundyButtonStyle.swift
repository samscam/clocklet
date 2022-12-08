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
            .font(.headline)
            .frame(maxWidth:.infinity)
            .foregroundColor(.white)
            .padding()
            .background(
                RoundedRectangle(cornerRadius: 20)
                    .fill(Color.accentColor)
                    .shadow(color: .primary.opacity(0.33), radius: configuration.isPressed ? 1.5 : 2)
            )
            .scaleEffect(configuration.isPressed ? 0.95 : 1)
            .animation(.easeIn(duration: 0.1), value: configuration.isPressed)
        
    }
    
}

extension Text {
    func lozenge() -> some View{
        return self.foregroundColor(.white)
        .padding()
        .background(Capsule().fill(Color.accentColor))
    }
}
