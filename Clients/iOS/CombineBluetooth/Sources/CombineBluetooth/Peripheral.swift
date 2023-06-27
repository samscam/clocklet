import Foundation
import CoreBluetooth
import Combine

// PERIPHERAL

public protocol AdvertisementMatcher: InnerPeripheralProtocol{
    
/**
 When the central searches for devices, it will match the uuids mentioned in the incoming advertisement packets against the ones in this list.
*/
    static var advertisedServiceUUIDs: [String]? { get }
    static var advertisedManufacturer: String? { get }
}

public extension AdvertisementMatcher {
    static var advertisedUUIDs: Set<CBUUID>? {
        guard let uuids = Self.advertisedServiceUUIDs else {
            return nil
        }
        return Set(uuids.map{ CBUUID(string:$0) })
    }
}

public extension AdvertisementMatcher {
    static func matches(_ adData: AdvertisementData)->Bool{
        var matches = true
        if let matchUUIDs = advertisedUUIDs, let peripheralAdvertisedUUIDs = adData.serviceUUIDs {
            if matchUUIDs.isDisjoint(with: peripheralAdvertisedUUIDs) { matches = false }
        }
        return matches
    }
}


public enum ConnectionState {
    case disconnected(error: Error? = nil)
    case connecting
    case connected
}

extension ConnectionState: Equatable {
    public static func == (lhs: ConnectionState, rhs: ConnectionState) -> Bool {
        switch (lhs,rhs){
        case (.disconnected,.disconnected):
            return true
        case (.connecting,.connecting):
            return true
        case (.connected,.connected):
            return true
        default:
            return false
        }
    }
    
    
}


open class Peripheral: PeripheralProtocol, ObservableObject {
    
    @Published public var state: ConnectionState = .disconnected(error: nil) {
        didSet{
            switch state {
            case .disconnected:
                self.invalidate()
            default:
                break
            }
        }
    }
    @Published public var name: String
    
    @Published public var advertisementData: AdvertisementData?
    
    weak var connection: Connection?
    
    public let uuid: UUID
    public var bag = Set<AnyCancellable>()
    
    public required init(uuid: UUID, name: String, connection: Connection){
        
        self.uuid = uuid
        self.name = name
        self.connection = connection
        
        findInnerObservables()

    }
    
    public init(uuid: UUID, name: String){
        
        self.uuid = uuid
        self.name = name
        
        findInnerObservables()
    }
    
    
    public func connect(){
        switch state {
        case .disconnected:
            willConnect?()
            connection?.connect()
        default:
            break
        }
    }
    
    /// This is an override point for mocking
    public var willConnect: (()->Void)?
    
    public func disconnect(){
        connection?.disconnect()
    }
    
    public func didDisconnect(){
        invalidate()
    }
    
    func invalidate(){
        serviceWrappers.forEach{$0.didInvalidate()}
    }


}

public protocol PeripheralProtocol: ObservableObject, InnerPeripheralProtocol {}

public protocol InnerPeripheralProtocol: AnyObject {
    var uuid: UUID { get }
    var name: String { get set }
    var state: ConnectionState { get set }
    init(uuid: UUID, name: String, connection: Connection)
    var objectWillChange: ObservableObjectPublisher { get }
    var bag:Set<AnyCancellable> { get set }
    var advertisementData: AdvertisementData? {get set}
    func didDisconnect()
}


public extension InnerPeripheralProtocol {
    var serviceUUIDs: [CBUUID]{
        let m = Mirror(reflecting: self)
        return m.children.compactMap{ $0.value as? HasUUID }.map{ $0.uuid }
    }
    
    var services: [InnerServiceProtocol]{
        let m = Mirror(reflecting: self)
        return m.children.compactMap{ $0.value as? InnerServiceProtocol }
    }
    
    internal var serviceWrappers: [ServiceWrapper]{
        let m = Mirror(reflecting: self)
        return m.children.compactMap { $0.value as? ServiceWrapper}
    }
    
    internal func serviceWrapper(for cbService: CBService) -> ServiceWrapper? {
        return serviceWrappers.first { (service) -> Bool in
            return service.uuid == cbService.uuid
        }
    }
    
    internal var innerObservables: [InnerObservable]{
        let m = Mirror(reflecting: self)
        return m.children.compactMap{ $0.value as? InnerObservable }
    }

    // To make Peripherals work as ObservableObjects, just like each @Service is @Published we cascade the objectWillChange notifications down the tree. Note that it will only see the @Services at the final level of subclassing, so you can't subclass a Peripheral twice.
    internal func findInnerObservables(){
      self.innerObservables.forEach { [weak self] (inner) in
        guard let self = self else { return }
          inner.objectWillChange.sink { _ in
              self.objectWillChange.send()
          }.store(in: &self.bag)
      }
    }
}

extension Peripheral: CustomDebugStringConvertible{
    public var debugDescription: String {
        return "\(name) \(advertisementData?.localName ?? "--") \(advertisementData?.manufacturerData?.manufacturerIdString ?? "--")"
    }
    
    
}
