//
//  BluetoothOverlayView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 06/08/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import CoreBluetooth
import CombineBluetooth
import Combine

struct BluetoothOverlayView: View{
    
    @StateObject var viewModel: BluetoothStatusViewModel = BluetoothStatusViewModel()
    
    @EnvironmentObject var central: Central
    
    
    var body: some View{
        ZStack{
            VStack(alignment: .center){
                viewModel.image
                    .renderingMode(.template)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    
                    .foregroundColor(Color(UIColor.systemFill))
                    .frame(width: 150, height: 150, alignment: .center)
                if let message = viewModel.message {
                    Text(message).bold().multilineTextAlignment(.center).padding()
                }
                if let buttonText = viewModel.buttonText, let closure = viewModel.buttonClosure {
                    Button(buttonText, action: closure).buttonStyle(RoundyButtonStyle())
                }

            }.padding()
                .frame(maxWidth:.infinity,maxHeight: .infinity)
            .background(Color(UIColor.systemBackground).opacity(0.6))
        }.onAppear {
            viewModel.central = central
        }
        
        
    }
}

struct BluetoothOverlayView_Previews: PreviewProvider {
    static let central = Central()
    static var previews: some View {
        BluetoothOverlayView().environmentObject(central)
            .onAppear{
                var delay: Double = 5
                for state in CBManagerState.allCases {
                    DispatchQueue.main.asyncAfter(deadline: .now()+delay){
                        central.state = state
                    }
                    delay = delay + 5
                }
            }
            .background(Color.green)
            .previewLayout(.fixed(width:400,height:500))
    }
}

class BluetoothStatusViewModel: ObservableObject {
    @Published var image: Image = Image("bluetooth-logo")
    @Published var message: String?
    @Published var showSettingsButton: Bool = false
    @Published var buttonText: String? = nil
    @Published var buttonClosure: (()->())? = nil
    
    private var bag = Set<AnyCancellable>()
    
    var central: Central? {
        didSet{
            bag = []
            central?.$state.sink { [weak self] state in
                switch state{

                case .poweredOn:
                    self?.image = Image(systemName: "eye")
                    self?.message = "Bluetooth is fine"
                    self?.buttonText = nil
                    self?.buttonClosure = nil
                case .unknown:
                    self?.image = Image(systemName:"questionmark.diamond")
                    self?.message = "Checking Bluetooth"
                    self?.buttonText = nil
                    self?.buttonClosure = nil
                case .poweredOff:
                    self?.image = Image("bluetooth-logo")
                    self?.message = "Bluetooth is switched off.\nPlease go into Bluetooth settings and switch it on."
                    self?.buttonText = nil
                    self?.buttonClosure = nil
                case .unauthorized:
                    self?.image = Image("bluetooth-logo")
                    self?.message = "Clocklet is not authorised to use Bluetooth on your phone. Please open settings and switch it on."
                    self?.buttonText = "Open Settings"
                    self?.buttonClosure = {
                        if let url = URL(string: UIApplication.openSettingsURLString) {
                            UIApplication.shared.open(url, options: [:], completionHandler: nil)
                        } else {
                            print("Failed to create settings url :/")
                        }
                    }
                    self?.showSettingsButton = true
                case .unsupported:
                    self?.image = Image(systemName:"bolt.slash.fill")
                    self?.message = "Bluetooth is unsupported on this device. Really sorry but you're going to have to find another way to configure your Clocklet."
                    self?.buttonText = nil
                    self?.buttonClosure = nil
                case .resetting:
                    self?.image = Image("bluetooth-logo")
                    self?.message = "Bluetooth is resetting..."
                    self?.buttonText = nil
                    self?.buttonClosure = nil
                default:
                    self?.image = Image(systemName:"questionmark.diamond")
                    self?.message = "Unexpected Bluetooth Status :/"
                    self?.buttonText = nil
                    self?.buttonClosure = nil
                }
            }.store(in: &bag)
        }
    }
    init(central: Central? = nil){
        self.central = central
    }
}



extension CBManagerState: @retroactive CaseIterable, @retroactive Identifiable{
    
    public var id: Int {
        return self.rawValue
    }
    
    public static var allCases: [CBManagerState] = [
        .poweredOff,
        .poweredOn,
        .resetting,
        .unauthorized,
        .unknown,
        .unsupported]
    
}
