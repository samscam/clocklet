//
//  ConfigItemView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 16/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI



struct ConfigItemView<Content>: View where Content: View {
    var iconSystemName: String
    var title: String
    var content: (()->Content)
    
    init(iconSystemName: String, title: String, content: @escaping ()->Content){
        self.iconSystemName = iconSystemName
        self.title = title
        self.content = content
    }
    
    var body: some View{
        HStack(alignment: .top,spacing:10){
            Image(systemName:iconSystemName).resizable().scaledToFit().frame(width: 40, height: 40, alignment: .center)
//                .background(Color.blue)
            
            VStack(alignment: .leading) {

                Text(title).font(.title).bold().lineLimit(4)
//                    .background(Color.green)
                
                content()
//                    .background(Color.yellow)
            }
            
        }.frame(minWidth: 0, maxWidth: .infinity,  alignment: Alignment.topLeading)
    }
}


struct ConfigItemView_Previews: PreviewProvider {
    static var previews: some View {
                     
        ConfigItemView(iconSystemName: "smiley", title: "Hello"){
            Text("I love you")
        }.previewLayout(.sizeThatFits)
    }
}
