import Foundation
import CoreBluetooth

/**
 A Connection represents a discovered bluetooth peripheral
 It may or may not actually be connected
 */

public class Connection: NSObject{
    
    
    public let cbPeripheral: CBPeripheral
    
    public var peripheral: InnerPeripheralProtocol? = nil // Don't really want this to be optional but hey...
    
    public var rssi: Int
    
    private weak var central: Central? = nil
    
    public init(cbPeripheral: CBPeripheral, advertisementData: [String: Any], rssi: Int, central: Central, knownPeripheralTypes: [Advertiser.Type]){
        
        self.cbPeripheral = cbPeripheral
        self.rssi = rssi
        self.central = central
        
        super.init()
        
        self.cbPeripheral.delegate = self
        
        
        let name = cbPeripheral.name ?? cbPeripheral.identifier.shortString
        
        // Discover a suitable peripheral class...
        // Hoping this happens on first shot through...
        if let advertisedServices = advertisementData[CBAdvertisementDataServiceUUIDsKey],
            let advertisedCBuuids = advertisedServices as? [CBUUID] {
            
            let advertiseduuids = Set(advertisedCBuuids)
            
            print("Advertised uuids for \(name): \(advertiseduuids)")
            
            
            if let firstKnownPeripheral = knownPeripheralTypes
                .first(where: { (peripheralType) -> Bool in
                    return peripheralType.advertisedUUIDs == advertiseduuids
                }) {
                print("🏴‍☠️ Found \(firstKnownPeripheral)")
                let actualPeripheral = firstKnownPeripheral.init(uuid: cbPeripheral.identifier, name: name, connection: self)
                self.peripheral = actualPeripheral
            }
        }
    }
    
    func update(advertisementData: [String : Any], rssi: Int){
        self.rssi = rssi
    }
    
    func connect(){
        peripheral?.state = .connecting
        central?.connect(to: self.cbPeripheral)
    }
    
    func disconnect(){
        central?.disconnect(from: self.cbPeripheral)
    }
    
    
    // Methods passed in from the CBCentralManagerDelegate
    func didConnect(){
        peripheral?.state = .connected
        cbPeripheral.discoverServices(nil)
    }
    
    func didDisconnect(error: Error?){
        peripheral?.state = .disconnected(error: error)
    }
    
    func didFailToConnect(error: Error?){}
    
}



extension Connection: CBPeripheralDelegate{
    public func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        
        // ERRORS???
        if let error = error {
            print(error)
            return
        }
        
        let cbPeripheral = peripheral // We shall never speak of this again
        
        // Have we got services?
        guard let cbServices = cbPeripheral.services else { return }
        
        // Map the CBServices onto our Services
        self.peripheral?.serviceWrappers.forEach { (serviceWrapper) in

            if let thisCBService = cbServices.first(where: { $0.uuid == serviceWrapper.uuid }) {
                var serviceWrapper = serviceWrapper
                serviceWrapper.cbService = thisCBService
                cbPeripheral.discoverCharacteristics(nil, for: thisCBService)

            }
        }
    }
    
    public func peripheralDidUpdateName(_ peripheral: CBPeripheral) {
        self.peripheral?.name = cbPeripheral.name ?? "No name :("
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didReadRSSI RSSI: NSNumber, error: Error?) {
        self.rssi = Int(truncating: RSSI)
    }
    
    
    public func peripheral(_ peripheral: CBPeripheral, didModifyServices invalidatedServices: [CBService]) {
        
        
        
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didOpen channel: CBL2CAPChannel?, error: Error?) {
        
    }
    
    // Descriptors
    public func peripheral(_ peripheral: CBPeripheral, didWriteValueFor descriptor: CBDescriptor, error: Error?) {
        
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor descriptor: CBDescriptor, error: Error?) {
        
    }
    
    // Characteristics
    public func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        
        guard let characteristics = service.characteristics else {return}
        
        if let svcWrapper = self.peripheral?.serviceWrapper(for: service) {
            svcWrapper.didDiscoverCharacteristics()
        }
        
        for characteristic in characteristics {
            peripheral.setNotifyValue(true, for: characteristic)
            print("Discovered char: ", characteristic)
            peripheral.readValue(for: characteristic)
        }
        
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didWriteValueFor characteristic: CBCharacteristic, error: Error?) {
        
    }
    
    // Sub-services
    public func peripheral(_ peripheral: CBPeripheral, didDiscoverIncludedServicesFor service: CBService, error: Error?) {
        
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        self.peripheral?.objectWillChange.send()
        if let service = self.peripheral?.serviceWrapper(for: characteristic.service){
            service.didUpdateValue(for: characteristic)
        }
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didDiscoverDescriptorsFor characteristic: CBCharacteristic, error: Error?) {
        
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didUpdateNotificationStateFor characteristic: CBCharacteristic, error: Error?) {
        
    }
    
    public func peripheralIsReady(toSendWriteWithoutResponse peripheral: CBPeripheral) {
        
    }
    
}
