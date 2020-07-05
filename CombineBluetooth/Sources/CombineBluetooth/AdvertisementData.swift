//
//  AdvertisementData.swift
//  
//
//  Created by Sam Easterby-Smith on 01/11/2019.
//

import Foundation
import CoreBluetooth

public struct AdvertisementData{
    public var localName: String?
    public var manufacturerData: ManufacturerData?
    public var serviceUUIDs: Set<CBUUID>?
    public var appearance: UInt16
    
    init(_ cbAdvertisementData: [String: Any]){
        if let localName = cbAdvertisementData[CBAdvertisementDataLocalNameKey] as? String {
            self.localName = localName
        }
        if let manufacturerData = cbAdvertisementData[CBAdvertisementDataManufacturerDataKey] as? Data {
            self.manufacturerData = ManufacturerData(manufacturerData)
        }
        if let uuids = cbAdvertisementData[CBAdvertisementDataServiceUUIDsKey] as? [CBUUID] {
            self.serviceUUIDs = Set(uuids)
        }
        if let appearance = cbAdvertisementData[CBAdvertisement] as? String {
            self.localName = localName
        }
    }
}

public struct ManufacturerData {
    init?(_ data: Data){
        guard data.count >= 3 else {
            return nil
        }
        // first two bytes are the manufacturer
        let mfr = data[0...1]
        self.manufacturerId = mfr
        
        // The rest is arbitrary
        let res = data[2...]
        self.residual = res
        
    }
    public var manufacturerId: Data?
    public var residual: Data?
}
