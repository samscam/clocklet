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
    
    private let locationProxy = LocationProxy()
    
    var bag: [AnyCancellable] = []
    
    required init(){
        
        $currentLocation.map{
                if let currentLocation = $0 {
                    return currentLocation.configured ? .configured : .notConfigured
                } else {
                    return .unknown
                }
            }
            .assign(to: \.isConfigured, on: self)
            .store(in: &bag)
        
    }
    
    deinit {
        locationProxy.disable()
    }

    @Published var isConfigured: ConfigState = .unknown
    
    @Characteristic("C8C7FF91-531A-4306-A68A-435374CB12A9") var currentLocation: ClockLocation?
    
    func setCurrentLocation(){

        locationProxy.locationPublisher.flatMap{ location in
            GeocoderProxy.futureReversePublisher(location)
        }
        .sink(receiveCompletion: { _ in
            
        }){ [weak self] (placemark) in
            if let lat = placemark.location?.coordinate.latitude,
               let lng = placemark.location?.coordinate.longitude,
               let timeZone = placemark.timeZone?.identifier,
               let placeName = placemark.locality {
                    self?.currentLocation = ClockLocation(configured:true,
                                                    lat: lat,
                                                    lng: lng,
                                                    timeZone: timeZone,
                                                    placeName: placeName)
            }
        }
        .store(in: &bag)
    }
}


struct ClockLocation: Codable, JSONCharacteristic, CustomStringConvertible{
    let configured: Bool
    let lat: Double?
    let lng: Double?
    let timeZone: String?
    let placeName: String?

    var description: String{
        return "\(String(lat ?? 0)),\(String(lng ?? 0))"
    }
    
    var location: CLLocation {
        return CLLocation(latitude: lat ?? 0, longitude: lng ?? 0)
    }
        
    static let nullIsland: ClockLocation = ClockLocation(configured: false, lat: 0, lng: 0, timeZone: "UTC", placeName: "Null Island")
}


