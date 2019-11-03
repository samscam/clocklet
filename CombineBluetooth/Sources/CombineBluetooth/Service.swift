import Foundation
import CoreBluetooth
import Combine

// SERVICES

public protocol ServiceWrapper: HasUUID {
    var cbService: CBService? { get set }
    func didUpdateValue(for: CBCharacteristic)
    func didDiscoverCharacteristics()
    func didInvalidate()
}

public protocol InnerServiceProtocol: class, HasUUID {
    static var uuid: CBUUID { get }
    var objectWillChange: ObservableObjectPublisher { get }
    init()
}

public protocol ServiceProtocol: InnerServiceProtocol, ObservableObject {
    
}

extension ServiceProtocol where Self: ObservableObject{
    func willChange(){
        self.objectWillChange.send()
    }
}


@propertyWrapper
public class Service<Value:ServiceProtocol>: ServiceWrapper {
  
    public var wrappedValue: Value {
        didSet {
            self.publisher.send(wrappedValue)
        }
    }
    
//    private var _value: Value?
//
//    public var wrappedValue: Value? {
//        get{
//            return _value
//        }
//    }
    
    public let uuid: CBUUID
    public var publisher: CurrentValueSubject<Value,Never>
    public var cbService: CBService?
    
    public init(wrappedValue value: Value){
        self.wrappedValue = value
//        self._value = value
        self.uuid = value.uuid
        self.publisher = CurrentValueSubject<Value,Never>(value)
    }
    
    deinit {
        print("Service deinited \(self.uuid)")
    }
    
    
    public var projectedValue: Service {
        return self
    }
    public func didDiscoverCharacteristics(){
        guard let cbCharacteristics = self.cbService?.characteristics else {
            return
        }
        
        for cbCharacteristic in cbCharacteristics {
            if let characteristicWrapper = self.characteristicWrapper(for: cbCharacteristic) {
                characteristicWrapper.cbCharacteristic = cbCharacteristic
            }
        }
    }
    
    public func didUpdateValue(for cbCharacteristic: CBCharacteristic){
        self.wrappedValue.objectWillChange.send()
        self.characteristicWrapper(for: cbCharacteristic)?.valueWasUpdated()
    }
    
    public func didInvalidate(){
//        self._value = nil
        self.cbService = nil
//        self.publisher.send(nil)
    }
    
    var characteristicWrappers: [CharacteristicWrapper]{
//        if let value = wrappedValue {
            let m = Mirror(reflecting: wrappedValue)
            return m.children.compactMap { $0.value as? CharacteristicWrapper}
//        } else {
//            return []
//        }
    }
    
    //
    func characteristicWrapper(for cbCharacteristic: CBCharacteristic)->CharacteristicWrapper?{
        return characteristicWrappers.first { (characteristic) -> Bool in
            return characteristic.uuid == cbCharacteristic.uuid
        }
    }
    
    
    
}


public extension ServiceProtocol{
    var uuid: CBUUID { return Self.uuid }
}

public extension ServiceProtocol {
    var characteristicUUIDs: [CBUUID]{
        let m = Mirror(reflecting: self)
        return m.children.compactMap{ $0.value as? HasUUID }.map{ $0.uuid }
    }
    

}

extension Optional: HasUUID where Wrapped: ServiceProtocol{
    public var uuid: CBUUID { return Wrapped.self.uuid }
}
