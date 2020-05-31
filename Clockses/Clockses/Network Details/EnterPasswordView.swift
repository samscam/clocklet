//
//  EnterPasswordView.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 29/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct EnterPasswordView: View {
    
    @Environment(\.presentationMode) var presentation
    
    let network: AvailableNetwork
    let networkService: NetworkService?
    
    @State private var password: String = ""
    @State private var textFieldActive = true
    
    var body: some View {
            ScrollView{
            VStack{
                
//                Text(self.network.ssid).font(.title).bold().lineLimit(2)
//                ResponderTextField(isFirstResponder: textFieldActive) {
//                    $0.borderStyle = .none
//                    $0.placeholder = "password"
//                }.padding()
//                    .cornerRadius(10)
//                    .overlay(RoundedRectangle(cornerRadius: 10, style: .circular).stroke(Color.primary, lineWidth: 1))
//                
                
                TextField("Password", text: self.$password)
                    .font(.title)
                    .padding()
                    .background(Color.gray)
                    .cornerRadius(10)
                    .overlay(RoundedRectangle(cornerRadius: 10, style: .circular).stroke(Color.primary, lineWidth: 1)

                )
                        
                HStack{
                    Button(action: {
                        self.presentation.wrappedValue.dismiss()
                    }) {
                        Text("Cancel")
                    }.accentColor(.red)
                    Spacer()
                    Button(action:{
                        try? self.networkService?.joinNetwork(self.network, psk: self.password)
                        self.presentation.wrappedValue.dismiss()
                    }){
                        Text("Connect").bold()
                    }
                    .accentColor(.green)
                }
                
            }.padding(10)
            .shadow(radius: 1)
        }.onAppear(){
            
        }
    }
}

struct EnterPasswordView_Previews: PreviewProvider {
    static var previews: some View {
        EnterPasswordView(network: AvailableNetwork(ssid: "Testnet", enctype: .wpa2enterprise, rssi: -15, channel: 2, bssid: "bums"), networkService: nil)
            .previewLayout(.sizeThatFits)
        
    }
}

struct ResponderTextField: UIViewRepresentable {

    typealias TheUIView = UITextField
    var isFirstResponder: Bool
    var configuration = { (view: TheUIView) in }

    func makeUIView(context: UIViewRepresentableContext<Self>) -> TheUIView { TheUIView() }
    func updateUIView(_ uiView: TheUIView, context: UIViewRepresentableContext<Self>) {
        _ = isFirstResponder ? uiView.becomeFirstResponder() : uiView.resignFirstResponder()
        configuration(uiView)
    }
}
