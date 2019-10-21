import Foundation


struct Network {
    let ssid: String
    let connected: Bool
    let rssi: Int?
    let authError: Bool?
}


struct NetworkService {
    
    let knownNetworks: [Network]
    
    func addNetwork(ssid: String, passphrase: String?) {
        
    }
    
    func removeNetwork(ssid: String){
        
    }
}

struct ClockService {
    
}

enum RainbowDisplayStyle {
    case rainbow
    case temp
}

struct Service {
    let uuid: UUID
    let characteristics: [Characteristic]
}
struct Characteristic {
    let uuid: UUID
    let value: Any
}
