//
//  LocationDetailsView.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 01/08/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import SwiftUI
import CoreLocation
import MapKit

class LocationDetailsViewModel: ObservableObject {
    
    
    private var _locationService: LocationService
    
    init(locationService: LocationService){
        _locationService = locationService
    }
}

struct PopularPlace: Identifiable, Hashable{
    var id: String { return name }
    let name: String
    let tzUTCOffset: Double
}

extension CurrentLocation {
    var coordinate: CLLocationCoordinate2D { return CLLocationCoordinate2D(latitude: lat ?? 0, longitude: lng ?? 0)}
}

extension CurrentLocation: Identifiable{
    var id: Double { return (lat ?? 0) * (lng ?? 0) }
}
struct LocationDetailsView: View {
    
    @EnvironmentObject var locationService: LocationService
    @EnvironmentObject var clock: Clock
     
    static let popularLocations: [PopularPlace] = [
        PopularPlace(name: "New York", tzUTCOffset: -10),
        PopularPlace(name: "London", tzUTCOffset: -10),
        PopularPlace(name: "Paris", tzUTCOffset: 1),
        PopularPlace(name: "Munich", tzUTCOffset: 3),
    ]
    
    @State var selectedLocation: PopularPlace?
    
    @State var region: MKCoordinateRegion = MKCoordinateRegion(center: .init(latitude:  53.480759, longitude: -2.242631),
                                                               span: MKCoordinateSpan(latitudeDelta: 0.02, longitudeDelta: 0.02))
    
    @State var annotations: [CurrentLocation] = [CurrentLocation(configured: true, lat: 53.480759, lng: -2.242631)]
    
    
    var body: some View {
        VStack{
            
            if #available(iOS 14.0,macOS 11.0, *) {
                Map(coordinateRegion: $region, showsUserLocation: true,
                    annotationItems: annotations){ annotation in
//                    MapPin(coordinate: annotation.coordinate)
                    MapAnnotation(coordinate: annotation.coordinate){
                        Image(clock.caseColor.imageName).resizable()
                            .aspectRatio(contentMode: .fit).frame(width: 140, height: 60, alignment: .center)
                    }
                }.frame(height: 200)
                    
            } else {
                MapView(coordinate: region.center)
            }
            
            Spacer()
            
            Button("Set to current location"){
                    self.locationService.setCurrentLocation()
                }
                .buttonStyle(RoundyButtonStyle())
            Spacer()
            
            ConfigItemView(icon: Image(systemName: "globe"), title: "Popular Locations") {
                
                List(LocationDetailsView.popularLocations, selection: $selectedLocation) { (place) in
                    PlaceRowView(place: place)
                }.listStyle(PlainListStyle())
            }
        }.padding()
        .navigationBarTitle( Text("Location Settings") )
    }
}

struct PlaceRowView: View {
    let place: PopularPlace
    var body: some View {
        HStack {
            Text(place.name)
        }
    }
}

struct LocationDetailsView_Previews: PreviewProvider {
    static var previews: some View {
        NavigationView(){
            LocationDetailsView()
        }
    }
}
