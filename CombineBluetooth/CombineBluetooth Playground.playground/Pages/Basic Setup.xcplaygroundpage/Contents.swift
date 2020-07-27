import CoreBluetooth
import Combine
import PlaygroundSupport


// Service examples
var str = ""

class NetworkService: ServiceProtocol {
    static var uuid: CBUUID = CBUUID(string:"68D924A1-C1B2-497B-AC16-FD1D98EDB41F")
    
    @Characteristic(CBUUID(string:"BEB5483E-36E1-4688-B7F5-EA07361B26A8")) var availableNetwork: Int? = 32
    @Characteristic(CBUUID(string:"CEB5483E-36E1-4688-B7F5-EA07361B26A8")) var wibbles: String = "Hello"
}

class InfoService: ServiceProtocol{
    static var uuid: CBUUID = CBUUID(string:"CCB5483E-36E1-4688-B7F5-EA07361B26A8")
    
    @Characteristic(CBUUID(string:"EEEEE83E-36E1-4688-B7F5-EA07361B26A8")) var hardwareRevision: Int? = nil
    @Characteristic(CBUUID(string:"FFFFF83E-36E1-4688-B7F5-EA07361B26A8")) var firmwareVersion: String? = nil
}

let netService = NetworkService()
netService.characteristicUUIDs
netService.availableNetwork
netService.availableNetwork = 5
netService.$availableNetwork.uuid

netService.wibbles = "Really"

netService.$wibbles.uuid

// Peripheral examples

class Clock: Peripheral {
    
    @Service var networkService: NetworkService?
    @Service var infoService: InfoService?
    
    // Rather than saying what is advertised, quite a few things seem to not advertise services and instead rely on manufacturer data
    static let advertised: [ServiceProtocol.Type] = [NetworkService.self]
    

}


let clock = Clock(uuid: UUID(), name:"Big Ben")
clock.services
clock.serviceUUIDs
clock.rawServices
Clock.advertisedUUIDs



//PlaygroundPage.current.needsIndefiniteExecution = true

let central = Central()
var connectionPub = central.discoverConnections(for: Clock.self)

var connectionSubscription: AnyCancellable? = connectionPub.sink(receiveCompletion: { (comp) in
    print("Completion")
}) { (connections) in
    print("**** I got connections ****")
    
    for connection in connections {
        print(" \(connection.cbPeripheral.identifier.shortString) \(connection.cbPeripheral.name ?? "No name") \(connection.peripheral.debugDescription)")
    }
}

DispatchQueue.main.asyncAfter(deadline: .now() + 30) {
    connectionSubscription = nil
}

