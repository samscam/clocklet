import Foundation
import CoreBluetooth
import Combine

// SERVICES

public protocol ServiceWrapper: HasUUID {
    var cbService: CBService? { get set }
    func didUpdateValue(for: CBCharacteristic)
    func didDiscoverCharacteristics()
}

public protocol InnerServiceProtocol: class, HasUUID {
    static var uuid: CBUUID { get }
    var objectWillChange: ObservableObjectPublisher { get }
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
    
    public let uuid: CBUUID
    public var publisher: CurrentValueSubject<Value,Never>
    public var cbService: CBService?
    
    public init(wrappedValue value: Value){
        
        self.wrappedValue = value
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
    
    var characteristicWrappers: [CharacteristicWrapper]{
        let m = Mirror(reflecting: wrappedValue)
        return m.children.compactMap { $0.value as? CharacteristicWrapper}
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
