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
import Combine

class LocationDetailsViewModel: ObservableObject {
    
    @Published var showMap: Bool = false
    
    @Published var currentLocation: CurrentLocation = .nullIsland
    
    @Published var annotations: [CurrentLocation] = []
    
    lazy var region = Binding<MKCoordinateRegion>(
        get:{ return MKCoordinateRegion(center: self.currentLocation.coordinate, latitudinalMeters: 5000, longitudinalMeters: 5000) },
        set:{ _ in }
    )
    
    
    private var _locationService: LocationService
    
    var bag = Set<AnyCancellable>()
    
    init(locationService: LocationService){
        _locationService = locationService
        _locationService.$currentLocation
            .compactMap{ $0 }
            .assign(to: \.currentLocation, on: self)
            .store(in: &bag)
        
        $currentLocation.map{ [$0] }.assign(to: \.annotations, on: self).store(in: &bag)
        
        _locationService.$isConfigured
            .map{ $0 == .configured }
            .assign(to: \.showMap, on: self)
            .store(in: &bag)
        
    }
    
    func setCurrentLocation(){
        _locationService.setCurrentLocation()
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
    
    @EnvironmentObject var viewModel: LocationDetailsViewModel
    @EnvironmentObject var clock: Clock
     
    static let popularLocations: [PopularPlace] = [
        PopularPlace(name: "New York", tzUTCOffset: -10),
        PopularPlace(name: "London", tzUTCOffset: -10),
        PopularPlace(name: "Paris", tzUTCOffset: 1),
        PopularPlace(name: "Munich", tzUTCOffset: 3),
    ]
    
    @State var selectedLocation: PopularPlace?
    
    
    var body: some View {
        ScrollView{
        VStack(alignment: .leading){
            if viewModel.showMap {
            if #available(iOS 14.0,macOS 11.0, *) {
                Map(coordinateRegion: viewModel.region, interactionModes: MapInteractionModes(), showsUserLocation: true,
                    annotationItems: viewModel.annotations){ annotation in
                    MapAnnotation(coordinate: annotation.coordinate){
                        Image(clock.caseColor.imageName).resizable()
                            .aspectRatio(contentMode: .fit).frame(width: 140, height: 60, alignment: .center)
                    }
                }.frame(height: 200)
                    
            } else {
                MapView(coordinate: viewModel.region.center.wrappedValue)
            }
            
            Spacer()
            }
            
            VStack(alignment:.leading){
                HStack{
                    Text("Place:").foregroundColor(.secondary)
                    Text(viewModel.currentLocation.placeName ?? "Unknown Place").bold().foregroundColor(.primary)
                }
                
                HStack{
                    Text("Time zone:").foregroundColor(.secondary)
                    Text(viewModel.currentLocation.timeZone ?? "Unknown time zone").bold().foregroundColor(.primary)
                }
            }
            
            Spacer()
            
            Button("Set to current location"){
                    self.viewModel.setCurrentLocation()
                }
                .buttonStyle(RoundyButtonStyle())
            
            Spacer()
            
            ConfigItemView(icon: Image(systemName: "globe"), title: "Popular Locations") {
                
                ForEach(LocationDetailsView.popularLocations) { (place) in
                    PlaceRowView(place: place)
                }.listStyle(PlainListStyle())
            }
        }.padding()
        }
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
