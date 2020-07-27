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

extension Data {

    init<T>(from value: T) {
        self = Swift.withUnsafeBytes(of: value) { Data($0) }
    }

    func to<T>(_ type: T.Type) -> T? where T: ExpressibleByIntegerLiteral {
        var value: T = 0
        guard count >= MemoryLayout.size(ofValue: value) else { return nil }
        _ = Swift.withUnsafeMutableBytes(of: &value, { copyBytes(to: $0)} )
        return value
    }
}

let u32: UInt32 = 0x00030005
(u32 << 16) >> 16
(u32 >> 16)

let u16: UInt16 = 0x05FF
u16 & 0xFF
(u16 >> 8) & 0xFF


let data = Data([0x08, 0xA5, 0x03, 0x00, 0x00, 0x08])
let val = data[0..<2]

val.to(UInt16.self)
data[2..<4].to(UInt16.self)
data[4..<6].to(UInt16.self)
