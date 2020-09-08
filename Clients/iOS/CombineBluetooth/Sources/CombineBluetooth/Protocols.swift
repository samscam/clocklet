import Foundation
import CoreBluetooth
import Combine

public protocol HasUUID{
    var uuid: CBUUID { get }
}



internal protocol InnerObservable {
    var objectWillChange: ObservableObjectPublisher {get}
}
