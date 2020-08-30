//
//  BluetoothOverlayView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 06/08/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import CoreBluetooth



struct BluetoothOverlayView: View{
    
    var bluetoothStatus: BluetoothStatusViewModel
    
    //    @EnvironmentObject var clockList: ClockList
    @Environment(\.colorScheme) var colorScheme: ColorScheme
    
    
    var body: some View{
        ZStack{
            VStack(alignment: .center){
                bluetoothStatus.image
                    .renderingMode(.template)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    
                    .foregroundColor(Color(UIColor.systemFill))
                    .frame(width: 150, height: 150, alignment: .center)
                
                Text(bluetoothStatus.message).bold().multilineTextAlignment(.center).padding()
                if let buttonText = bluetoothStatus.buttonText, let closure = bluetoothStatus.buttonClosure {
                    Button(buttonText, action: closure).buttonStyle(RoundyButtonStyle())
                }

            }.padding()
            .frame(maxWidth:.infinity)
            .background(Color(UIColor.systemBackground).opacity(0.6))
        }
        
        
    }
}

struct BluetoothOverlayView_Previews: PreviewProvider {
    static var previews: some View {
        Group{
            ForEach(CBManagerState.allCases){ cbState in
                BluetoothOverlayView(bluetoothStatus: BluetoothStatusViewModel(state: cbState))
            }.previewLayout(.sizeThatFits)
        }.background(Color.green)
    }
}

struct BluetoothStatusViewModel {
    var image: Image
    var message: String
    var showSettingsButton: Bool = false
    var buttonText: String? = nil
    var buttonClosure: (()->())? = nil
    
    init(state: CBManagerState){
        switch state{

        case .poweredOn:
            image = Image(systemName: "eye")
            message = "Bluetooth is fine"

        case .unknown:
            image = Image(systemName:"questionmark.diamond")
            message = "Checking Bluetooth"
        case .poweredOff:
            image = Image("bluetooth-logo")
            message = "Bluetooth is switched off.\nPlease go into Bluetooth settings and switch it on."
        case .unauthorized:
            image = Image("bluetooth-logo")
            message = "Clocklet is not authorised to use Bluetooth on your phone. Please open settings and switch it on."
            buttonText = "Open Settings"
            buttonClosure = {
                UIApplication.shared.open(URL(string: UIApplication.openSettingsURLString)!, options: [:], completionHandler: nil)
            }
            showSettingsButton = true
        case .unsupported:
            image = Image(systemName:"bolt.slash.fill")
            message = "Bluetooth is unsupported on this device. Really sorry but you're going to have to find another way to configure your Clocklet."
        case .resetting:
            image = Image("bluetooth-logo")
            message = "Bluetooth is resetting..."
            
        default:
            image = Image(systemName:"questionmark.diamond")
            message = "Unexpected Bluetooth Status :/"
        }
    }
}



extension CBManagerState: CaseIterable, Identifiable{
    
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
