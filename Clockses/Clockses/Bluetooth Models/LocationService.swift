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
    
    private let geocoderProxy = GeocoderProxy()
    private let locationProxy = LocationProxy()
    
    var bag: [AnyCancellable] = []
    
    required init(){
        
        self.$currentLocation.compactMap{$0?.location}
        .flatMap{ location in
            self.geocoderProxy.futureReversePublisher(location)
        }
        .catch { (error) in
            Just(nil)
        }
        .assign(to: \.placemark, on: self)
        .store(in: &bag)
    }
    
    deinit {
        locationProxy.disable()
    }

    static let uuid = CBUUID(string: "87888F3E-C1BF-4832-9823-F19C73328D30")
    
    @Characteristic(CBUUID(string:"C8C7FF91-531A-4306-A68A-435374CB12A9")) var currentLocation: CurrentLocation?
    
    @Published var placemark: CLPlacemark?
    
    
    func setCurrentLocation(){

        locationProxy.locationPublisher.sink(receiveCompletion: { (completion) in

        }) { [weak self] (location) in
            self?.currentLocation = CurrentLocation(lat: location.coordinate.latitude, lng: location.coordinate.longitude)
        }.store(in: &bag)
    }
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

