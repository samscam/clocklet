//
//  ContentView.swift
//  ClockletMac
//
//  Created by Sam Easterby-Smith on 12/01/2023.
//  Copyright © 2023 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        VStack {
            Image(systemName: "globe")
                .imageScale(.large)
                .foregroundColor(.accentColor)
            Text("Hello, world!")
        }
        .padding()
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
