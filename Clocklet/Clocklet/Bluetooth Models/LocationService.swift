//
//  LocationService.swift
//  Clocklet
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
        .replaceError(with: nil)
        .assign(to: \.placemark, on: self)
        .store(in: &bag)
        
        $currentLocation.map{
            if let currentLocation = $0 {
                return currentLocation.configured ? .configured : .notConfigured
            } else {
                return .unknown
            }
        }.replaceError(with: .unknown)
        .assign(to: \.isConfigured, on: self)
        .store(in: &bag)
        
    }
    
    deinit {
        locationProxy.disable()
    }

    @Published var isConfigured: ConfigState = .unknown
    
    static let uuid = CBUUID(string: "87888F3E-C1BF-4832-9823-F19C73328D30")
    
    @Characteristic(CBUUID(string:"C8C7FF91-531A-4306-A68A-435374CB12A9")) var currentLocation: CurrentLocation?
    
    @Published var placemark: CLPlacemark?
    
    
    func setCurrentLocation(){

        locationProxy.locationPublisher.sink(receiveCompletion: { (completion) in

        }) { [weak self] (location) in
            self?.currentLocation = CurrentLocation(configured:true,
                                                    lat: location.coordinate.latitude,
                                                    lng: location.coordinate.longitude)
        }.store(in: &bag)
    }
}


struct CurrentLocation: Codable, JSONCharacteristic, CustomStringConvertible{
    let configured: Bool
    let lat: Double?
    let lng: Double?

    
    var description: String{
        return "\(String(lat ?? 0)),\(String(lng ?? 0))"
    }
    
    var location: CLLocation {
        return CLLocation(latitude: lat ?? 0, longitude: lng ?? 0)
    }
    
}

