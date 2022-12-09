import Foundation

// DATA CONVERTIBLE
// hooray for stackoverflow
// https://stackoverflow.com/questions/38023838/round-trip-swift-number-types-to-from-data

public protocol DataConvertible{
    init?(data: Data)
    var data: Data { get }
}

public extension DataConvertible where Self: Numeric{

    init?(data: Data) {
        var value: Self = 0
        guard data.count == MemoryLayout.size(ofValue: value) else { return nil }
        _ = withUnsafeMutableBytes(of: &value, { data.copyBytes(to: $0)} )
        self = value
    }

    var data: Data {
        return withUnsafeBytes(of: self) { Data($0) }
    }
}

extension Int: DataConvertible{}
extension Double: DataConvertible{}
extension Float: DataConvertible{}
extension UInt8: DataConvertible{}


extension Bool: DataConvertible{
    
    public init?(data: Data) {
        guard data.count == 1 else { return nil }
        let byte: UInt8 = data[0]
        self = (byte != 0)
    }

    public var data: Data {
        var data = Data(capacity: 1)
        data.append(self ? 1 : 0)
        return data
    }
}


extension String: DataConvertible{
    public init?(data: Data){
        self.init(data: data, encoding: .utf8)
    }
    public var data: Data {
        return Data(self.utf8)
    }
}

extension RawRepresentable where Self:DataConvertible, RawValue: DataConvertible{
    public init?(data: Data){
        if let raw = RawValue.init(data: data) {
            self.init(rawValue:raw)
        }
        return nil
    }
    
    public var data: Data {
        return self.rawValue.data
    }
}

// Can't remember why this is commented out but it's interesting!
//extension Optional: DataConvertible where Wrapped: DataConvertible{
//    public init?(data: Data) {
//        if let w = Wrapped.init(data: data){
//            self = .some(w)
//        } else {
//            self = nil
//        }
//    }
//    public var data: Data {
//        switch self {
//        case .some(let val):
//            return val.data
//        case .none:
//            return Data()
//        }
//    }
//}

public protocol JSONCharacteristic: Codable, DataConvertible {}

public extension DataConvertible where Self: JSONCharacteristic {
    init?(data: Data){
        let decoder = JSONDecoder()
        do {
            self = try decoder.decode(Self.self, from: data)
        } catch {
            print(error)
            print(String(data: data, encoding: .utf8) ?? "No data")
            return nil
        }
    }
    var data: Data {
        let encoder = JSONEncoder()
        do {
            let data = try encoder.encode(self)
            return data
        } catch {
            print(error)
            return Data()
        }
    }
}

extension Array: DataConvertible where Element:Codable {
    
}

extension Array: JSONCharacteristic where Element:Codable{
    
}
