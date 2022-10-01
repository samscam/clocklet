//
//  LocationDetailsViewModel.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 01/10/2022.
//  Copyright © 2022 Spotlight Kid Ltd. All rights reserved.
//

import Foundation

import CoreLocation
import MapKit
import Combine

class LocationDetailsViewModel: ObservableObject {
    
    @Published var showMap: Bool = false
    
    @Published var currentLocation: ClockLocation = .nullIsland
    
    @Published var region: MKCoordinateRegion?
    
    @Published var annotations: [ClockLocation] = []
    
    private var _locationService: LocationService
    
    var bag = Set<AnyCancellable>()
    
    init(locationService: LocationService){
        _locationService = locationService
        _locationService.$currentLocation
            .compactMap{ $0 }
            .assign(to: \.currentLocation, on: self)
            .store(in: &bag)
        
        $currentLocation.map{ [$0] }.assign(to: \.annotations, on: self).store(in: &bag)
        
        $currentLocation.map{ currentLocation in
            MKCoordinateRegion(center: currentLocation.coordinate, latitudinalMeters: 5000, longitudinalMeters: 5000) }
            .assign(to: \.region, on: self)
            .store(in: &bag)
        
        _locationService.$isConfigured
            .map{ $0 == .configured }
            .assign(to: \.showMap, on: self)
            .store(in: &bag)
        
    }
    
    func setCurrentLocation(){
        _locationService.setCurrentLocation()
    }
    func setLocation(_ clockLocation: ClockLocation){
        _locationService.currentLocation = clockLocation
    }
}

struct PopularPlace: Identifiable, Hashable{
    var id: String { return name }
    let name: String
    let tzUTCOffset: Double
}

extension ClockLocation {
    var coordinate: CLLocationCoordinate2D { return CLLocationCoordinate2D(latitude: lat ?? 0, longitude: lng ?? 0)}
}

extension ClockLocation: Identifiable{
    var id: String { return placeName ?? "Nowhere" }
}
