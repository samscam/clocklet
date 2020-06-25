import Foundation
import CoreBluetooth
import Combine

// CHARACTERISTICS

public protocol CharacteristicWrapper: class, HasUUID {
    
    /// This is called internally by the service wrapper when it reads a change of value for the characteristic
    func didUpdateValue(error: Error?)
    
    /// This is called internally by the service wrapper after a write transaction has taken place
    func didWriteValue(error: Error?)
    func invalidate()
    
    var cbCharacteristic: CBCharacteristic? { get set }
}


enum CharacteristicState<Value> {
    case unknown
    case writing(value: Value)
    case read(value: Value)
}

@propertyWrapper
public class Characteristic<Value: DataConvertible>: CharacteristicWrapper, Publisher, ObservableObject {
    
    public enum CharacteristicError: Error{
        case noCBCharacteristic
        case couldNotWriteValue
    }

    public let uuid: CBUUID
    
    public let subject = CurrentValueSubject<Value?,Error>(nil)
    
    public typealias Output = Value?
    public typealias Failure = Error
    
    public func receive<S>(subscriber: S) where S : Subscriber, Characteristic.Failure == S.Failure, Characteristic.Output == S.Input {
        subject.receive(subscriber: subscriber)
    }
    
    public var cbCharacteristic: CBCharacteristic?
    
    public init(wrappedValue value: Value? = nil, _ uuid: CBUUID){
        self._value = value
        subject.send(value)
        self.uuid = uuid
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
            
            if let cbCharacteristic = cbCharacteristic,
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
    
    // READING VALUES FROM REMOTE
    
    // Called internally when the remote value changed
    public func didUpdateValue(error: Error?){
        if let cbc = cbCharacteristic,
            let data = cbc.value {
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
//        let transaction = writeClosureFifo.removeFirst()
//        _value = transaction.newValue // arguably shouldn't do this
//        transaction.closure?(transaction.newValue,error)
//
    }
//
//    public func setValue(_ newValue: Value?, completion: ((Value?, Error?)->())? ){
//        guard let cbc = cbCharacteristic else {
//            completion?(_value,CharacteristicError.noCBCharacteristic)
//            return
//        }
//        let transaction = WriteTransaction(newValue: newValue, closure: completion)
//        writeClosureFifo.append(transaction)
//
//        cbc.service.peripheral.writeValue(newValue.data, for: cbc, type: .withResponse)
//    }

    
    
    public var projectedValue: Characteristic {
        return self
    }
    
    public func invalidate(){
        cbCharacteristic = nil
        wrappedValue = nil
        
    }
}
