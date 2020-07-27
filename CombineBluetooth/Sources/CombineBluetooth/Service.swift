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
public class Service<Value:ServiceProtocol>: ServiceWrapper, Publisher, ObservableObject, InnerObservable {

  
    public var wrappedValue: Value? {
        get{
            return self._subject.value
        }
        set {
            self._subject.send(newValue)
        }
    }
    
    public let uuid: CBUUID
    
    private var bag = Set<AnyCancellable>()
    private var innerBag = Set<AnyCancellable>()
    
    public var cbService: CBService?
    
    public init(wrappedValue value: Value? = nil, _ uuid: String){
        
        self.uuid = CBUUID(string: uuid)
        
        self.wrappedValue = value
        
        _subject.sink{ newValue in
            Swift.print("New value of Service \(self) : \(newValue) -- Object will change")
            self.objectWillChange.send()
            self.innerBag.removeAll()
            if let serviceValue = newValue {
                serviceValue.innerObservables.forEach{ characteristicWrapper in
                    characteristicWrapper.objectWillChange.sink{_ in
                        Swift.print("Service \(self) caught ObjectWillChange from \(characteristicWrapper)")
                        self.objectWillChange.send()
                        Swift.print("... passing that back up to \(serviceValue)")
                        serviceValue.objectWillChange.send()
                    }.store(in: &self.innerBag)
                }
            }
            }.store(in: &bag)
    }
    
    
    public var projectedValue: Service {
        return self
    }
    
    // Publisher conformance
    private let _subject = CurrentValueSubject<Value?,Never>(nil)
    
    public typealias Output = Value?
    public typealias Failure = Never
    
    public func receive<S>(subscriber: S) where S : Subscriber, Service.Failure == S.Failure, Service.Output == S.Input {
        _subject.receive(subscriber: subscriber)
    }
    
    
    // ServiceWrapper conformance
    
    public func didDiscoverCharacteristics(){
        guard let cbCharacteristics = self.cbService?.characteristics else {
            return
        }
        
        for cbCharacteristic in cbCharacteristics {
            
            if let characteristicWrapper = wrappedValue?.characteristicWrapper(for: cbCharacteristic) {
                characteristicWrapper.cbCharacteristic = cbCharacteristic
            }
        }
    }
    
    public func didUpdateValue(for cbCharacteristic: CBCharacteristic, error: Error?){
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
    
    internal var characteristicWrappers: [CharacteristicWrapper]{
        let m = Mirror(reflecting: self)
        return m.children.compactMap { $0.value as? CharacteristicWrapper}
    }
    
    internal func characteristicWrapper(for cbCharacteristic: CBCharacteristic)->CharacteristicWrapper?{
        return characteristicWrappers.first { (characteristic) -> Bool in
            return characteristic.uuid == cbCharacteristic.uuid
        }
    }
    
    internal var innerObservables: [InnerObservable]{
        let m = Mirror(reflecting: self)
        return m.children.compactMap{ $0.value as? InnerObservable }
    }

}
