//
//  LocationProxy.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 02/11/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CoreLocation
import Combine

enum LocationProxyError: Error {
    case authorizationDenied
    case authorizationRestricted
}

class LocationProxy: NSObject, CLLocationManagerDelegate {
    let locationManager = CLLocationManager()
    private let locationPublisher = PassthroughSubject<CLLocation, Error>()
    var publisher: AnyPublisher<CLLocation, Error>
    var shouldFetchLocation: Bool = false

    override init(){
        publisher = locationPublisher.eraseToAnyPublisher()
        super.init()
        locationManager.delegate = self
        
    }
    
    deinit{
        print("Location proxy deinit")
    }
    
    func enable(){
        switch CLLocationManager.authorizationStatus() {
        case .authorizedAlways, .authorizedWhenInUse:
            locationManager.requestLocation()
        case .notDetermined:
            shouldFetchLocation = true
            locationManager.requestWhenInUseAuthorization()
        default:
            break
        }
        
        
    }
    func disable(){
        locationManager.stopUpdatingLocation()
    }
    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        if let lastLocation = locations.last {
            locationPublisher.send(lastLocation)
        }
    }
    
    func locationManager(_ manager: CLLocationManager, didChangeAuthorization status: CLAuthorizationStatus) {
        switch status {
        case .authorizedWhenInUse, .authorizedAlways:
            if shouldFetchLocation {
                locationManager.requestLocation()
                shouldFetchLocation = false
            }
        case .denied:
            locationPublisher.send(completion: Subscribers.Completion.failure(LocationProxyError.authorizationDenied))
        case .restricted:
            locationPublisher.send(completion: Subscribers.Completion.failure(LocationProxyError.authorizationRestricted))
        default:
            break
        }
    }
    
    func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        locationPublisher.send(completion: Subscribers.Completion.failure(error))
    }
    
}
