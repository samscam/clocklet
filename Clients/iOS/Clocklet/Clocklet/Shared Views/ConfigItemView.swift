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
    var disclosure: Bool
    var content: (()->Content)
    
    
    init(icon: Image, iconColor: Color? = nil, title: String, disclosure: Bool = false, @ViewBuilder content: @escaping (()->Content)){
        self.icon = icon
        self.iconColor = iconColor
        self.title = title
        self.content = content
        self.disclosure = disclosure
    }
    
    var body: some View{
        VStack(alignment: .leading, spacing: 10) {
            Spacer()
            HStack(alignment: .center,spacing:10){
                
                icon.resizable().scaledToFit().frame(maxWidth: 40, alignment: .center).foregroundColor(self.iconColor ?? Color("Black-ish"))
                Text(title).font(.title).bold().lineLimit(4).foregroundColor(Color("Black-ish"))
                Spacer()
                if (disclosure){
                    Image(systemName: "chevron.right")
                }
            }
            .frame(minWidth:0, maxWidth: .infinity)
            .contentShape(Rectangle())
            
            content()
                .fixedSize(horizontal: false, vertical: true)
                .frame(maxWidth: .infinity, maxHeight: .infinity)
                .padding(.leading, 50)
            
        }.frame( maxWidth: .infinity,  maxHeight: .infinity, alignment: Alignment.topLeading)
        
        
    }
}


struct ConfigItemView_Previews: PreviewProvider {
    static var previews: some View {
        VStack{
            ConfigItemView(icon: Image(systemName: "link.circle.fill"), title: "Hello"){EmptyView()}
            
            ConfigItemView(icon: Image(systemName: "trash.fill"), title: "Throw it away"){
                Text("""
Nullam quis risus eget urna mollis ornare vel eu leo. Etiam porta sem malesuada magna mollis euismod. Nullam id dolor id nibh ultricies vehicula ut id elit. Donec id elit non mi porta gravida at eget metus. Nullam id dolor id nibh ultricies vehicula ut id elit.

Maecenas faucibus mollis interdum. Cras mattis consectetur purus sit amet fermentum. Nullam id dolor id nibh ultricies vehicula ut id elit. Fusce dapibus, tellus ac cursus commodo, tortor mauris condimentum nibh, ut fermentum massa justo sit amet risus. Curabitur blandit tempus porttitor. Etiam porta sem malesuada magna mollis euismod. Vestibulum id ligula porta felis euismod semper.
""")
                
            }
            
            ConfigItemView(icon: Image(systemName: "smiley"), title: "Hello"){
                Text("I love you")
            }
            
        }.padding()
        
    }
}
