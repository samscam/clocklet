//
//  ErrorView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 20/12/2022.
//  Copyright © 2022 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct ErrorView: View {
    let message: String
    
    init(_ error: Error){
        self.message = error.localizedDescription
    }
    init(_ message: String){
        self.message = message
    }

    
    var body: some View {
        HStack(spacing:10){
            Image(systemName: "exclamationmark.triangle")
                .resizable()
                .scaledToFit()
                .frame(maxWidth:50)
                .foregroundColor(.black)
            Text(message)
                .foregroundColor(.black)
                .lineLimit(nil)
                .fixedSize(horizontal: false, vertical: true)
                .frame(maxWidth:.infinity)
                
        }.padding()
            .background(
                RoundedRectangle(cornerRadius: 8)
                    .fill(Color.init(hue: 0.12, saturation: 0.6, brightness: 1))
                    
            ).padding(.horizontal, 40)
        
        Spacer(minLength:30)
    }
}
