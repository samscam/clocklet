//
//  OtherSettingsView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 17/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct OtherSettingsView: View {
    var body: some View {
        ConfigItemView(icon: Image(systemName:"dial"), title: "Settings") {
             Text("Hello World!")
            
        }
    }
}

struct OtherSettingsView_Previews: PreviewProvider {
    static var previews: some View {
        OtherSettingsView()
    }
}
