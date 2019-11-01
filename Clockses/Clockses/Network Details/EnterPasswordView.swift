//
//  EnterPasswordView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 29/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct EnterPasswordView: View {
    let network: AvailableNetwork
    let networkService: NetworkService?
    
    @State private var password: String = ""
    
    var body: some View {
        VStack{
            
            Text(network.ssid).font(.title).bold().lineLimit(2)

            SecureField("Password", text: $password)
                .font(.title)
                .padding()
                .background(Color.gray)
                .cornerRadius(10)
                .overlay(RoundedRectangle(cornerRadius: 10, style: .circular).stroke(Color.black, lineWidth: 3)
                    
            )
            HStack{
                Button(action: {}) {
                    Text("Cancel")
                }.accentColor(.red)
                Spacer()
                Button(action:{
                    try? self.networkService?.joinNetwork(self.network, psk: self.password)
                    
                }){
                    Text("Join").bold()
                }
                .accentColor(.green)
            }
            
        }.padding()
    }
}

struct EnterPasswordView_Previews: PreviewProvider {
    static var previews: some View {
        EnterPasswordView(network: AvailableNetwork(ssid: "Testnet", enctype: .wpa2enterprise, rssi: -15, channel: 2, bssid: "bums"), networkService: nil)
            .previewLayout(.sizeThatFits)
        
    }
}
