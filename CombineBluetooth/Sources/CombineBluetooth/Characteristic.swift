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

    public let uuid: CBUUID
    public var shouldNotify: Bool = false {
        didSet{
            updateNotifyState()
        }
    }
    
    private var bag = Set<AnyCancellable>()
    
    private let subject = CurrentValueSubject<Value?,Never>(nil)
    
    private let _sendQueue = PassthroughSubject<Value,Never>()
    
    internal var cbCharacteristic: CBCharacteristic? {
        didSet{
            updateNotifyState()
        }
    }
    
    private func updateNotifyState(){
        if let cbCharacteristic = cbCharacteristic{
            cbCharacteristic.service.peripheral.setNotifyValue(shouldNotify, for: cbCharacteristic)
        }
    }
    
    public init(wrappedValue value: Value? = nil, _ uuid: String){
        self._value = value
        self.uuid = CBUUID(string: uuid)
        
        // Throttling here for the benefit of sliders - so if values are changed rapidly they aren't sent too fast
        _sendQueue
            .throttle(for: 0.1, scheduler: RunLoop.main, latest: true)
            .sink { (value) in
                self.writeValueToPeipheral(value: value)
            }.store(in: &bag)
        
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
            if let newValue = newValue {
                _sendQueue.send(newValue)
            }
        }
    }
    
    private func writeValueToPeipheral(value: Value){
        if let cbCharacteristic = self.cbCharacteristic{
            
            let data = value.data
            
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
