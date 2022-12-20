//: [Previous](@previous)

import Foundation
import SwiftUI
import PlaygroundSupport
import CombineBluetooth

/// function for de-optionalising bindings
@available(iOS 13,*)
public func ??<T>(lhs: Binding<Optional<T>>, rhs: T) -> Binding<T> {
    Binding(
        get: { lhs.wrappedValue ?? rhs },
        set: { lhs.wrappedValue = $0 }
    )
}

@available(iOS 13,*)
public func ??(lhs: Binding<Optional<String>>, rhs: String) -> Binding<String>{
    Binding(
        get: { lhs.wrappedValue ?? rhs },
        set: {
            if $0 == "" { // or possibly whitespace
                lhs.wrappedValue = nil
            } else {
                lhs.wrappedValue = $0
            }
            
        }
    )
}

struct Orb: Equatable, DataConvertible, JSONCharacteristic {
    var enabled: Bool = false
    var beans: Beans = Beans()
}

struct Beans: DataConvertible, Equatable, JSONCharacteristic {
    var has: String?
    var cocoa: Bool = false
    var mung: Int = 25
}

class OrbHolder: ObservableObject {
    @Published var orb: Orb = Orb()
    
//    @Characteristic(UUID().uuidString) var orbCharacteristic: Orb?
}


struct OrbView: View {
    @StateObject var orbHolder = OrbHolder()
    
    var body: some View{
        VStack{
            Toggle("Enabled", isOn: $orbHolder.orb.enabled)
                .onChange(of: orbHolder.orb){ value in
                    print(orbHolder.orb)
                }
            TextField("Has", text: $orbHolder.orb.beans.has ?? "")
                
        }
    }
}



PlaygroundPage.current.setLiveView(OrbView())

//: [Next](@next)
