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
    init(_ data: Data){
        // first two bytes are the manufacturer
        let mfr = data[0...1]
        self.manufacturerId = mfr
        print("Manufacturer: ",mfr)
        
        let res = data[2...]
        self.residual = res
        print("Residual: ", String(data:res, encoding: .utf8))
    
        
        
        // The rest is arbitrary
    }
    public var manufacturerId: Data?
    public var residual: Data?
}
