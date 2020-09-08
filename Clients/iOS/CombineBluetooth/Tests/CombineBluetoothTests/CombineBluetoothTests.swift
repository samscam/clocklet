import XCTest
import CoreBluetooth
@testable import CombineBluetooth

final class CombineBluetoothTests: XCTestCase {
    func testExample() {
        // This is an example of a functional test case.
        // Use XCTAssert and related functions to verify your tests produce the correct
        // results.
        XCTAssertEqual(CombineBluetooth().text, "Hello, World!")
    }

    static var allTests = [
        ("testExample", testExample),
    ]
}


class MockCBCentralManager: CBCentralManager {
    
}
