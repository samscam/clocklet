//
//  LocationServiceView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 17/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct LocationServiceView: View {
    var body: some View {
        ConfigItemView(iconSystemName: "location", title: "Withington, Manchester, UK") {
             Text("Hello World!")
            
        }
       
    }
}

struct LocationServiceView_Previews: PreviewProvider {
    static var previews: some View {
        LocationServiceView()
    }
}
