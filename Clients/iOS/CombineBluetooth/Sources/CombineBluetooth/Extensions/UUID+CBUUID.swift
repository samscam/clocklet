import Foundation
import CoreBluetooth

extension UUID: @retroactive ExpressibleByStringLiteral, @retroactive ExpressibleByExtendedGraphemeClusterLiteral {
    public init(stringLiteral value: String) {
        self.init(uuidString: value)!
    }
}

public extension UUID {
    static func ==(lhs: UUID, rhs: CBUUID) -> Bool{
        return lhs.uuidString == rhs.uuidString
    }
    
    var shortString: String {
        let uuidString = self.uuidString
        let index6 = uuidString.index(uuidString.startIndex, offsetBy: 6)
        return String(uuidString[...index6])
    }
    
    var asCBUUID: CBUUID{
        let string = self.uuidString
        return CBUUID(string: string)
    }
}

public extension CBUUID {
    var UUIDValue: UUID? {
        get {
            if self.data.count != MemoryLayout<uuid_t>.size { return nil }
            return self.data.withUnsafeBytes {
                            (buffer: UnsafeRawBufferPointer) -> UUID in
                let pointer = buffer.bindMemory(to: uuid_t.self)
                return UUID(uuid:pointer.baseAddress!.pointee)
            }
        }
    }
}
