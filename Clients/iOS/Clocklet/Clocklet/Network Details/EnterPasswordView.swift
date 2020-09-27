//
//  EnterPasswordView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 29/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI

struct EnterPasswordView: View {
    
    @Environment(\.presentationMode) var presentation
    
    var network: AvailableNetwork? = nil
    let networkService: NetworkService?
    
    @State private var networkSSID: String = ""
    @State private var password: String = ""
    @State private var textFieldActive = true
    
    var body: some View {
            ScrollView{
            VStack{
                
                if (network == nil){
                    TextField("Network SSID", text: self.$networkSSID)
                        .disableAutocorrection(true)
                        .autocapitalization(.none)
                        .font(.title)
                        .padding()
                        .background(Color.gray)
                        .cornerRadius(10)
                        .overlay(RoundedRectangle(cornerRadius: 10, style: .circular).stroke(Color.secondary, lineWidth: 1)

                    )
                }

                if self.network?.enctype != .open {
                    TextField("Password", text: self.$password)
                        .disableAutocorrection(true)
                        .autocapitalization(.none)
                        .font(.title)
                        .padding()
                        .background(Color.gray)
                        .cornerRadius(10)
                        .overlay(RoundedRectangle(cornerRadius: 10, style: .circular).stroke(Color.secondary, lineWidth: 1)

                    )
                }
                        
                
                    Button(action:{
                        if let network = self.network {
                            self.networkService?.joinNetwork(network, psk: self.password)
                        
                        } else {
                            self.networkService?.joinNetwork(self.networkSSID, psk: self.password)
                        }
                        
                        self.presentation.wrappedValue.dismiss()
                    }){
                        Text("Connect").bold()
                    }
                    
                    .accentColor(.green)
                    .buttonStyle(RoundyButtonStyle())
                    
                
                
            }.padding(10)
            .shadow(radius: 1)
        }.onAppear(){
            
        }.navigationBarTitle("Join \(network?.ssid ?? "other network")")
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
