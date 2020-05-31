//
//  LocationService.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 22/10/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CoreBluetooth
import CombineBluetooth
import Combine
import CoreLocation

class LocationService: ServiceProtocol {
    
    required init(){}
    let objectWillChange = ObservableObjectPublisher() // If we leave it to the synthesised one it doesn't work :/

    static let uuid = CBUUID(string: "87888F3E-C1BF-4832-9823-F19C73328D30")
    
    @Characteristic(CBUUID(string:"C8C7FF91-531A-4306-A68A-435374CB12A9")) var currentLocation: CurrentLocation?
    

}

enum LocationStatus: String, DataConvertible {
    case notSet
    case set
}

struct CurrentLocation: Codable, JSONCharacteristic, CustomStringConvertible{
    let lat: Double
    let lng: Double

    
    var description: String{
        return "\(String(lat)),\(String(lng))"
    }
    
    var location: CLLocation {
        return CLLocation(latitude: lat, longitude: lng)
    }
    
}


enum GeocoderExtensionError: Error{
    case noResults
}

class GeocoderProxy {
    
    func futureReversePublisher(_ location: CLLocation) -> AnyPublisher<String, Error> {
        return Future { promise in
            CLGeocoder().reverseGeocodeLocation(location) { (placemarks, error) in
                if let error = error {
                    promise(.failure(error))
                    return
                }
                if let firstPlace = placemarks?.first {
                    var outputArr: [String] = []
//                    if let name = firstPlace.name {
//                        outputArr.append(name)
//                    }
                    if let locality = firstPlace.subLocality {
                        outputArr.append(locality)
                    }
//                    if let country = firstPlace.administrativeArea {
//                        outputArr.append(country)
//                    }
                    let resolved = outputArr.joined(separator: ", ")
                    promise(.success(resolved))
                    return
                }
                promise(.failure(GeocoderExtensionError.noResults))
            }
        }.eraseToAnyPublisher()
    }
}
