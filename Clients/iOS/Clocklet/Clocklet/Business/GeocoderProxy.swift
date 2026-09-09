//
//  GeocoderProxy.swift
//  Clocklet
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

struct GeocoderProxy {
    
    static func reverseGeocode(_ location: CLLocation) async throws -> CLPlacemark {
        
        try await withCheckedThrowingContinuation { continuation in
            CLGeocoder().reverseGeocodeLocation(location) { placemarks, error in
                if let error {
                    continuation.resume(throwing: error)
                    return
                }
                if let firstPlace = placemarks?.first {
                    continuation.resume(returning: firstPlace)
                    return
                }
                
                continuation.resume(throwing: GeocoderExtensionError.noResults)
                return
            }
        }
    }
    
}
