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

public protocol InnerServiceProtocol: class {
    init()
}

public protocol ServiceProtocol: InnerServiceProtocol, ObservableObject where ObjectWillChangePublisher == ObservableObjectPublisher {
    
}


@propertyWrapper
public class Service<Value:ServiceProtocol>: ServiceWrapper, Publisher {

  
    public var wrappedValue: Value? {
        didSet {
            self.subject.send(wrappedValue)
        }
    }
    
    public let uuid: CBUUID
    

    public var cbService: CBService?
    

    
    public init(wrappedValue value: Value? = nil, _ uuid: String){
        self.wrappedValue = value
        self.uuid = CBUUID(string: uuid)
    }
    
    
    public var projectedValue: Service {
        return self
    }
    
    // Publisher conformance
    private let subject = CurrentValueSubject<Value?,ServiceError>(nil)
    
    public typealias Output = Value?
    public typealias Failure = ServiceError
    
    public func receive<S>(subscriber: S) where S : Subscriber, Service.Failure == S.Failure, Service.Output == S.Input {
        subject.receive(subscriber: subscriber)
    }
    
    
    // ServiceWrapper conformance
    
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
    }
    
    
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
