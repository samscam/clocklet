import Foundation
import CoreBluetooth
import Combine

// SERVICES

public enum ServiceError: Error {
    case invalidated
}

internal protocol ServiceWrapper: HasUUID {
    var cbService: CBService? { get set }
    func didUpdateValue(for: CBCharacteristic, error: Error?)
    func didWriteValue(for: CBCharacteristic, error: Error?)
    func didDiscoverCharacteristics()
    
    func didDiscover()
    func didInvalidate()
}

public protocol InnerServiceProtocol: class, HasUUID {
    static var uuid: CBUUID { get }
    init()
}

public protocol ServiceProtocol: InnerServiceProtocol, ObservableObject where ObjectWillChangePublisher == ObservableObjectPublisher {
    
}


@propertyWrapper
public class Service<Value:ServiceProtocol>: ServiceWrapper {

  
    public var wrappedValue: Value? {
        didSet {
            self.publisher.send(wrappedValue)
        }
    }
    
    public let uuid: CBUUID
    public var publisher: CurrentValueSubject<Value?,ServiceError>
    public var cbService: CBService?
    
    public init(wrappedValue value: Value?){
        self.wrappedValue = value
        self.uuid = value.uuid
        self.publisher = CurrentValueSubject<Value?,ServiceError>(value)
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
            wrappedValue?.objectWillChange.send()
            if let characteristicWrapper = wrappedValue?.characteristicWrapper(for: cbCharacteristic) {
                characteristicWrapper.cbCharacteristic = cbCharacteristic
            }
        }
    }
    
    public func didUpdateValue(for cbCharacteristic: CBCharacteristic, error: Error?){
        wrappedValue?.objectWillChange.send()
        wrappedValue?.characteristicWrapper(for: cbCharacteristic)?.didUpdateValue(error: error)
    }
    
    public func didWriteValue(for characteristic: CBCharacteristic, error: Error?) {
        if let characteristicWrapper = wrappedValue?.characteristicWrapper(for: characteristic) {
            characteristicWrapper.didWriteValue(error: error)
        }
    }
    
    public func didDiscover(){
        self.wrappedValue = .init()
    }
    
    public func didInvalidate(){
        wrappedValue?.characteristicWrappers.forEach{ $0.invalidate() }
        self.cbService = nil
        self.wrappedValue = nil
        self.publisher.send(completion: .failure(.invalidated))
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
    
    var characteristicWrappers: [CharacteristicWrapper]{
        let m = Mirror(reflecting: self)
        return m.children.compactMap { $0.value as? CharacteristicWrapper}
    }
    
    //
    func characteristicWrapper(for cbCharacteristic: CBCharacteristic)->CharacteristicWrapper?{
        return characteristicWrappers.first { (characteristic) -> Bool in
            return characteristic.uuid == cbCharacteristic.uuid
        }
    }

}

extension Optional: HasUUID where Wrapped: ServiceProtocol{
    public var uuid: CBUUID { return Wrapped.self.uuid }
}
