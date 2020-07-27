import Foundation
import CoreBluetooth
import Combine

// CHARACTERISTICS



internal protocol CharacteristicWrapper: class, HasUUID {
    
    /// This is called internally by the service wrapper when it reads a value for the characteristic
    func didUpdateValue(error: Error?)
    
    /// This is called internally by the service wrapper after a write transaction has taken place
    func didWriteValue(error: Error?)
    
    
    func invalidate()
    
    var cbCharacteristic: CBCharacteristic? { get set }
}



@propertyWrapper
public class Characteristic<Value: DataConvertible>: CharacteristicWrapper, ObservableObject, InnerObservable {
    
    public enum CharacteristicError: Error{
        case noCBCharacteristic
        case couldNotWriteValue
    }

    public let uuid: CBUUID
    
    private let subject = CurrentValueSubject<Value?,Never>(nil)
    
    public var cbCharacteristic: CBCharacteristic?
    
    public init(wrappedValue value: Value? = nil, _ uuid: String){
        self._value = value
        self.uuid = CBUUID(string: uuid)
    }
    
    private var _value: Value? {
        willSet{
            objectWillChange.send()
        }
        didSet{
            subject.send(_value)
        }
    }

    public var wrappedValue: Value? {
        get{
            return _value
        }
        set{
            
            _value = newValue
            
            if let cbCharacteristic = self.cbCharacteristic,
                let data = _value?.data {
                
                // check if we can actually write to this characteristic...
                if (cbCharacteristic.properties.contains(.write)){
                    // these can crash if disconnected :(
                    cbCharacteristic.service.peripheral.writeValue(data, for: cbCharacteristic, type: .withResponse)
                    
                } else if (cbCharacteristic.properties.contains(.writeWithoutResponse)){
                    cbCharacteristic.service.peripheral.writeValue(data, for: cbCharacteristic, type: .withoutResponse)
                } else {
                    //we can't throw here... report this somehow...
                }
                
            }
        }
    }
    
    public var projectedValue: Characteristic {
        return self
    }
    
    public func invalidate(){
        cbCharacteristic = nil
        wrappedValue = nil
    }
    
    // READING VALUES FROM REMOTE
    
    // Called internally when the remote value changed
    internal func didUpdateValue(error: Error?){
        if let cbc = cbCharacteristic,
            let data = cbc.value {
            // Set the value and trigger ui updates
            // But don't send the value back to the server
            self._value = Value(data: data)
            
        }
    }
    
    
    // WRITING VALUES TO REMOTE
    
    struct WriteTransaction{
        let newValue: Value?
        let closure: ((Value?,Error?)->())?
    }
    
    var writeClosureFifo: [WriteTransaction] = []
    
    // Called internally when the local value has been written
    public func didWriteValue(error: Error?){
    }
    
    

}


extension Characteristic: Publisher {
    public typealias Output = Value?
    public typealias Failure = Never

    public func receive<S>(subscriber: S) where S : Subscriber, Characteristic.Failure == S.Failure, Characteristic.Output == S.Input {
       subject.receive(subscriber: subscriber)
    }
}
