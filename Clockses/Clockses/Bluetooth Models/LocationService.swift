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

class LocationService: ServiceProtocol, ObservableObject{
    let objectWillChange = ObservableObjectPublisher()
    
    func willChange() {
        self.objectWillChange.send()
    }
    
    static let uuid = CBUUID(string: "87888F3E-C1BF-4832-9823-F19C73328D30")
    
    @Characteristic(CBUUID(string:"C8C7FF91-531A-4306-A68A-435374CB12A9")) var currentLocation: CurrentLocation? = nil
    
}

struct CurrentLocation: Codable, JSONCharacteristic, CustomStringConvertible{
    let lng: Double
    let lat: Double
    
    var description: String{
        return "\(String(lat)),\(String(lng))"
    }
    
    var location: CLLocation {
        return CLLocation(latitude: lat, longitude: lng)
    }
}

struct LocationViewGeocoding {
    let geocoder = CLGeocoder()
    init(){

    }
    func publishedPlace(location: CLLocation) -> AnyPublisher<CLPlacemark,Never>{
        geocoder.reverseGeocodeLocation(location) { (placemarks, error) in
            
        }
    }
}
