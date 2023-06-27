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
    }
}

public struct ManufacturerData {
    init?(_ data: Data){
        guard data.count >= 2 else {
            return nil
        }
        
        // first two bytes are the manufacturer
        let mfr = data[0...1]
        self.manufacturerId = mfr
        
        guard data.count >= 3 else {
            return
        }
        
        // The rest is arbitrary
        let res = data[2...]
        self.residual = res
        
    }
    public var manufacturerId: Data?
    public var residual: Data?
    
    public var manufacturerIdString: String? {
        return manufacturerId?.map { String(format: "%02hhx", $0) }.joined()
    }
}
