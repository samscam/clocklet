//: [Previous](@previous)

import Foundation

var str = "Hello, playground"

public protocol DataConvertible{
    init?(data: Data)
    var data: Data { get }
}

//: [Next](@next)
extension Int32: DataConvertible{
    
    public init?(data: Data) {
        var value: Self = 0
        guard data.count == MemoryLayout.size(ofValue: value) else { return nil }
        _ = withUnsafeMutableBytes(of: &value, { data.copyBytes(to: $0)} )
        self = value
    }

    public var data: Data {
        return withUnsafeBytes(of: self) { Data($0) }
    }
}


let one: Int32 = 14

let oneData = one.data
oneData

let oneBackAgain = Int32(data:oneData)



