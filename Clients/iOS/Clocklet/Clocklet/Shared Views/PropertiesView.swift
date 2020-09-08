//
//  PropertiesView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 28/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI


struct Prop: Identifiable {
    let title:String
    let value:String
    
    init(_ title: String, _ value: String){
        self.title = title
        self.value = value
    }
    
    var id: String {
        return title
    }
}

struct PropertiesView: View{
    
    var properties: [Prop]
    
    var body: some View {
        HStack(spacing:5){
            VStack(alignment: .leading, spacing: 5){
                ForEach(properties){ prop in
                    Text(prop.title + ":")
                }
            }
            VStack(alignment: .leading, spacing: 5){
                ForEach(properties){ prop in
                    Text(prop.value).bold()
                }
            }
        }.font(.caption)
    }
}
struct PropertiesView_Previews: PreviewProvider {
    static var previews: some View {
        
        PropertiesView(properties:[Prop("IP","192.168.1.5"),
                                   Prop("Signal","-20"),
                                   Prop("Security","Closed")])
            
            .previewLayout(.sizeThatFits)
    }
}
