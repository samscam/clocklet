//
//  GeocoderProxy.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 07/06/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import CoreLocation
import Combine

enum GeocoderExtensionError: Error{
    case noResults
}

class GeocoderProxy {
    
    func futureReversePublisher(_ location: CLLocation) -> AnyPublisher<CLPlacemark?, Error> {
        return Future { promise in
            CLGeocoder().reverseGeocodeLocation(location) { (placemarks, error) in
                if let error = error {
                    promise(.failure(error))
                    return
                }
                if let firstPlace = placemarks?.first {
                    promise(.success(firstPlace))
                    return
                }
                promise(.failure(GeocoderExtensionError.noResults))
            }
        }.eraseToAnyPublisher()
    }
}
