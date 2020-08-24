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
        
    static let nullIsland = ClockLocation(configured: false, lat: 0, lng: 0, timeZone: "UTC", placeName: "Null Island")
    static let manchester = ClockLocation(configured: true, lat: 53.480709, lng: -2.234380, timeZone: "Europe/London", placeName: "Manchester")
    static let newYork = ClockLocation(configured: true, lat: 40.712776, lng: -74.005974, timeZone: "America/New_York", placeName: "New York")
    static let london = ClockLocation(configured: true, lat: 51.507351, lng: -0.127758, timeZone: "Europe/London", placeName:"London")
    static let paris = ClockLocation(configured: true, lat: 48.856613, lng: 2.352222, timeZone: "Europe/Paris", placeName: "Paris")
    static let munich = ClockLocation(configured: true, lat: 48.135124, lng: 11.581981, timeZone: "Europe/Berlin", placeName: "Munich")
    static let sanfrancisco = ClockLocation(configured: true, lat: 37.774929, lng: -122.419418, timeZone: "America/Los_Angeles", placeName: "San Francisco")
    static let melbourne = ClockLocation(configured: true, lat: -37.813629, lng: 144.963058, timeZone: "Australia/Melbourne", placeName: "Melbourne")
    static let helipark = ClockLocation(configured: true, lat: 43.51, lng: 170.9, timeZone: "Pacific/Auckland", placeName: "Helipark New Zealand")
    static let chatham = ClockLocation(configured: true, lat: -43.911720, lng: -176.506340, timeZone: "Pacific/Chatham", placeName: "Chatham Island")
    static let mumbai = ClockLocation(configured: true, lat: 19.076090, lng: 72.877426, timeZone: "Asia/Kolkata", placeName: "Mumbai")
}


