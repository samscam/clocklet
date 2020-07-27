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
    
    
    public init(cbPeripheral: CBPeripheral, advertisementData: [String: Any], rssi: Int, central: Central, knownPeripheralTypes: [AdvertisementMatcher.Type]){
        
        self.cbPeripheral = cbPeripheral
        self.rssi = rssi
        self.central = central

        super.init()
        
        let adData = AdvertisementData(advertisementData)
        
        
        self.cbPeripheral.delegate = self
        let name = cbPeripheral.name ?? cbPeripheral.identifier.shortString
        
        // Discover a suitable peripheral class...
        // Hoping this happens on first shot through...
        if let advertiseduuids = adData.serviceUUIDs {
            
            print("Advertised uuids for \(name): \(advertiseduuids)")
            
            
            if let firstKnownPeripheral = knownPeripheralTypes
                .first(where: { (peripheralType) -> Bool in
                    return peripheralType.advertisedUUIDs == advertiseduuids
                }) {
                print("🏴‍☠️ Found \(firstKnownPeripheral)")
                let actualPeripheral = firstKnownPeripheral.init(uuid: cbPeripheral.identifier, name: name, connection: self)
                actualPeripheral.advertisementData = adData
                self.peripheral = actualPeripheral
            }
        }
    }
    
    func update(advertisementData: [String : Any], rssi: Int){
        self.rssi = rssi
        peripheral?.advertisementData = AdvertisementData(advertisementData)
        peripheral?.objectWillChange.send()
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
        peripheral?.didDisconnect()
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
        cbServices
            .forEach{
                if var wrapper = self.peripheral?.serviceWrapper(for: $0) {
                    wrapper.didDiscover()
                    wrapper.cbService = $0
                    cbPeripheral.discoverCharacteristics(nil, for: $0)
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
        
        // Remove the services we know to be invalid
        invalidatedServices
            .compactMap{ self.peripheral?.serviceWrapper(for: $0) }
            .forEach{
                $0.didInvalidate()
            }
        
        // Re-discover anything new
        peripheral.discoverServices(nil)
        
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
        if let serviceWrapper = self.peripheral?.serviceWrapper(for:characteristic.service) {
            serviceWrapper.didWriteValue(for:characteristic, error: error)
        }
        
    }
    
    // Sub-services
    public func peripheral(_ peripheral: CBPeripheral, didDiscoverIncludedServicesFor service: CBService, error: Error?) {
        
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        if let service = self.peripheral?.serviceWrapper(for: characteristic.service){
            service.didUpdateValue(for: characteristic, error: error)
        }
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didDiscoverDescriptorsFor characteristic: CBCharacteristic, error: Error?) {
        
    }
    
    public func peripheral(_ peripheral: CBPeripheral, didUpdateNotificationStateFor characteristic: CBCharacteristic, error: Error?) {

    }
    
    public func peripheralIsReady(toSendWriteWithoutResponse peripheral: CBPeripheral) {
        
    }
    
}
