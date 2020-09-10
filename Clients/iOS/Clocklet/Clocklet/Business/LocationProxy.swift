//
//  LocationProxy.swift
//  Clocklet
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
    
    private let authSubject = CurrentValueSubject<CLAuthorizationStatus, Never>(CLLocationManager.authorizationStatus())
    
    lazy var authPublisher: AnyPublisher<CLAuthorizationStatus, Never> = {
       return authSubject
        .eraseToAnyPublisher()
    }()
    
    private let locationSubject = PassthroughSubject<CLLocation, Error>()
    
    lazy var locationPublisher: AnyPublisher<CLLocation, Error> = {
        return locationSubject.handleEvents(receiveSubscription: { [weak self] _ in
            print("Starting updates")
            self?.enable()
            self?.locationManager.startUpdatingLocation()
        }, receiveCompletion: { [weak self] _ in
            print("Completion - stopping updates")
            self?.locationManager.stopUpdatingLocation()
        }, receiveCancel: { [weak self] in
            print("Cancel - stopping updates")
            self?.locationManager.stopUpdatingLocation()
        }).eraseToAnyPublisher()
    }()

    override init(){
        super.init()
        locationManager.desiredAccuracy = kCLLocationAccuracyThreeKilometers
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
            locationSubject.send(lastLocation)
            disable()
        }
    }
    
    func locationManager(_ manager: CLLocationManager, didChangeAuthorization status: CLAuthorizationStatus) {
        authSubject.send(status)
    }
    
    func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        locationSubject.send(completion: Subscribers.Completion.failure(error))
    }
    
}
