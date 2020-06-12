import Foundation
import CoreBluetooth
import Combine

/**
 CENTRAL should probably be called CentralManager
*/

public class Central: NSObject, ObservableObject {

    private let _cbCentralManager: CBCentralManager = CBCentralManager()
    
    @Published var state: CBManagerState = .unknown
    @Published var connections: [UUID:Connection] = [:]
    @Published public var isScanning: Bool = false
    
    func scan(forServices services: Set<CBUUID>? = nil)->AnyPublisher<[Connection],Never>{
        
        return $state
            .filter({ (state) -> Bool in
                return state == .poweredOn
            }).handleEvents(receiveOutput: { state in 
                print("CENTRAL: starting scan")
                self.isScanning = true
                self._cbCentralManager.scanForPeripherals(withServices: services?.compactMap({$0}) , options: nil) // Ends up with empty array rather than nil???
        }, receiveCompletion: { (completion) in
            print("CENTRAL: Completed: Stopping scan")
            self._cbCentralManager.stopScan()
            self.isScanning = false
        }, receiveCancel: {
            print("CENTRAL: Cancelled: Stopping scan")
            self._cbCentralManager.stopScan()
            self.isScanning = false
        })

        .combineLatest($connections, { (state, output) -> [Connection] in
            return output.map{$1}
        }).eraseToAnyPublisher()
    }
    
    public override init(){
        super.init()
        _cbCentralManager.delegate = self
        state = _cbCentralManager.state
    }
    
    let allCBPeripherals: CurrentValueSubject<[CBPeripheral],Error> = .init([])
    
    public func discoverConnections<T: PeripheralProtocol & Advertiser>(for peripheralType: T.Type) -> AnyPublisher<[Connection],Never> {
        registerPeripheralType(peripheralType)
        let cons = scan(forServices: peripheralType.advertisedUUIDs).map{ $0.filter { connection -> Bool in
            return connection.peripheral is T
            }}.eraseToAnyPublisher()
        return cons
        
    }
    
    public func disconnectAllDevices(){
        connections.forEach { (_, connection) in
            connection.disconnect()
        }
    }
    
    private var knownPeripheralTypes: [Advertiser.Type] = []
    
    
    private func registerPeripheralType<T: Advertiser>(_ type: T.Type){
        
        // Types don't conform to Equatable but they do allow == so we have to do this...
        if !knownPeripheralTypes.contains(where: { (knownType) -> Bool in
            return knownType == type
        }){
            knownPeripheralTypes.append(type)
        }
        
    }
    
    // Delegatey methods...
    
    func connect(to peripheral: CBPeripheral){
        _cbCentralManager.connect(peripheral, options: nil)
    }
    
    func disconnect(from peripheral: CBPeripheral){
        _cbCentralManager.cancelPeripheralConnection(peripheral)
    }
    

}

extension Central: CBCentralManagerDelegate {
    public func centralManagerDidUpdateState(_ central: CBCentralManager) {
        state = central.state
    }
    
    public func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi: NSNumber) {

        let uuid = peripheral.identifier
        
        if let existing = connections[uuid] {
            existing.update(advertisementData: advertisementData, rssi: Int(truncating: rssi))
        } else {
            // Create a new Connection
            let newConnection = Connection(cbPeripheral: peripheral,
                                           advertisementData: advertisementData,
                                           rssi: Int(truncating: rssi),
                                           central: self,
                                           knownPeripheralTypes: knownPeripheralTypes)
            
            connections[uuid] = newConnection
        }
    }
    
    public func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        if let connection = connections[peripheral.identifier]{
            connection.didConnect()
        }
    }
    public func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        if let connection = connections[peripheral.identifier]{
            connection.didDisconnect(error: error)
        }
    }
    
    public func centralManager(_ central: CBCentralManager, willRestoreState dict: [String : Any]) {
        // Don't know what this does
        
    }
    
    public func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        if let connection = connections[peripheral.identifier]{
            connection.didFailToConnect(error: error)
        }
    }
}
