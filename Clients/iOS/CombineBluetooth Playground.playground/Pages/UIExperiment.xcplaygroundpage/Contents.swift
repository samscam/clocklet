//: [Previous](@previous)

import SwiftUI
import PlaygroundSupport

struct Finger: Identifiable {
    var id: String { return name }
    
    var name: String
    var biography: String?
    var raised: Bool = false
}

class MainViewModel: ObservableObject{
    @Published var lastFinger: Finger?
    @Published var fingers: [Finger]
    @Published var showingAll: Bool = false
    
    init(){
        fingers = [
            Finger(name: "Daddy"),
            Finger(name: "Mummy"),
            Finger(name: "Baby"),
            Finger(name: "Brother"),
            Finger(name: "Sister")
        ]
    }
}


struct RootInterface:View {
    @StateObject var main: MainViewModel = MainViewModel()
    @Namespace private var rootNamespace
    
    var body: some View {
        if let lastFinger = main.lastFinger {
            
                Image(systemName: "arrow.up")
                    .resizable()
                    .scaledToFit()
                    .frame(width:30,height:30)
                    .onTapGesture {
                        withAnimation{
                            main.lastFinger = nil
                        }
                    }.transition(.scale)
            fingerView(lastFinger).matchedGeometryEffect(id: lastFinger.name, in: rootNamespace)
                FingerDetailView(finger: lastFinger).transition(.move(edge: .bottom))
            
        } else {
            ForEach(main.fingers){ finger in
                fingerView(finger).matchedGeometryEffect(id: finger.name, in: rootNamespace).onTapGesture {
                    withAnimation{
                        main.lastFinger = finger
                    }
                }
            }
            
        }
    }
    
    func fingerView(_ finger: Finger) -> some View {
        VStack{
            Text(finger.name)
        }
        .padding()
        .background{Rectangle().foregroundColor(.red)}
        .padding()
    }
}

struct FingerDetailView: View {
    var finger: Finger
    var body: some View {
        Text(finger.biography ?? "No biography available for this finger")
            .padding()
            .background{Rectangle().foregroundColor(.yellow)}
            .padding()
    }
}


PlaygroundPage.current.setLiveView(
    RootInterface().frame(width:400,height:800)
)


//: [Next](@next)
