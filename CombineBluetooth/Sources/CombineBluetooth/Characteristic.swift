import Foundation
import CoreBluetooth
import Combine

// CHARACTERISTICS


public protocol CharacteristicWrapper: class, HasUUID {
    
    // This is called internally by the service wrapper when the value changes
    func valueWasUpdated()
    
    var cbCharacteristic: CBCharacteristic? { get set }
}

@propertyWrapper
public class Characteristic<Value: DataConvertible>: CharacteristicWrapper {
    
    var value: Value {
        didSet{
            self.publisher.send(value)
        }
    }
    
    public let uuid: CBUUID
    public var publisher: CurrentValueSubject<Value?,Never>
    
    public var cbCharacteristic: CBCharacteristic?
    
    public init(wrappedValue value: Value, _ uuid: CBUUID){
        self.publisher = CurrentValueSubject<Value?,Never>(value)
        self.value = value
        self.uuid = uuid
    }
    
    public func valueWasUpdated(){
        // erm... erm... yeah...
        self.publisher.send(wrappedValue)
    }
    
    public var wrappedValue: Value {
        get {
            if let cbc = cbCharacteristic,
                let data = cbc.value,
                let value = Value(data: data) {
                self.value = value
            }
            return value
        }
        set {
            value = newValue
            if let cbc = cbCharacteristic{
                cbc.service.peripheral.writeValue(value.data, for: cbc, type: .withResponse)
            }
        }
    }

    public var projectedValue: Characteristic {
        return self
    }
}
