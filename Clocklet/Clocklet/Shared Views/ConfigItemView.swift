//
//  ConfigItemView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 16/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI


struct ConfigItemView<Content:View>: View {
    var icon: Image
    var iconColor: Color?
    var title: String
    var content: (()->Content)
    
    
    init(icon: Image, iconColor: Color? = nil, title: String, content: @escaping (()->Content)){
        self.icon = icon
        self.iconColor = iconColor
        self.title = title
        self.content = content
    }
    
    var body: some View{
        VStack(alignment: .leading, spacing: 10) {
            HStack(alignment: .center,spacing:10){
                
                icon.resizable().scaledToFit().frame(width: 30, height: 40, alignment: .center).foregroundColor(self.iconColor ?? Color("Black-ish"))
                Text(title).font(.title).bold().lineLimit(4).foregroundColor(Color("Black-ish"))
            }
            
            content().padding(.leading, 40)
        }.frame(minWidth: 0, maxWidth: .infinity,  alignment: Alignment.topLeading)
    }
}


struct ConfigItemView_Previews: PreviewProvider {
    static var previews: some View {
        Group{
            ConfigItemView(icon: Image(systemName: "link.circle.fill"), title: "Hello"){EmptyView()}.previewLayout(.sizeThatFits)
            
            ConfigItemView(icon: Image(systemName: "trash.fill"), title: "Throw it away"){
                Text("""
Nullam quis risus eget urna mollis ornare vel eu leo. Etiam porta sem malesuada magna mollis euismod. Nullam id dolor id nibh ultricies vehicula ut id elit. Donec id elit non mi porta gravida at eget metus. Nullam id dolor id nibh ultricies vehicula ut id elit.

Maecenas faucibus mollis interdum. Cras mattis consectetur purus sit amet fermentum. Nullam id dolor id nibh ultricies vehicula ut id elit. Fusce dapibus, tellus ac cursus commodo, tortor mauris condimentum nibh, ut fermentum massa justo sit amet risus. Curabitur blandit tempus porttitor. Etiam porta sem malesuada magna mollis euismod. Vestibulum id ligula porta felis euismod semper.
""")
            }.previewLayout(.sizeThatFits)
            
            ConfigItemView(icon: Image(systemName: "smiley"), title: "Hello"){
                Text("I love you")
            }.previewLayout(.sizeThatFits).environment(\.colorScheme, .dark)
            
        }
    }
}
