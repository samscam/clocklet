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
    
    @Published var currentLocation: ClockLocation = .nullIsland
    
    @Published var annotations: [ClockLocation] = []
    
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
    var id: Double { return (lat ?? 0) * (lng ?? 0) }
}
struct LocationDetailsView: View {
    
    @EnvironmentObject var viewModel: LocationDetailsViewModel
    @EnvironmentObject var clock: Clock
     
    static let popularLocations: [ClockLocation] = [
        .manchester,.newYork,.london,.paris,.munich,.sanfrancisco,.melbourne,.chatham,.mumbai,.helipark
    ]
    
    
    var body: some View {
        ScrollView{
        VStack(alignment: .leading){
            if viewModel.showMap {
            
                ZStack{
                    if #available(iOS 14.0,macOS 11.0, *) {
                        Map(coordinateRegion: viewModel.region, interactionModes: MapInteractionModes(), showsUserLocation: true,
                            annotationItems: viewModel.annotations){ annotation in
                            MapAnnotation(coordinate: annotation.coordinate){
                                Image(clock.caseColor.imageName).resizable()
                                    .aspectRatio(contentMode: .fit).frame(width: 140, height: 60, alignment: .center)
                            }
                        }.opacity(0.6).blendMode(.luminosity)
                    } else {
                        MapView(coordinate: viewModel.region.center.wrappedValue)
                    }
                    if let placeName = viewModel.currentLocation.placeName{
                        VStack(alignment: .center){
                        HStack(alignment:.top){
                            Text(placeName).font(.largeTitle).fontWeight(.black).multilineTextAlignment(.center).shadow(color: Color(UIColor.systemBackground), radius:5)
                        }.padding(5)
                        Spacer()
                        }.padding(5)
                    }
                }.frame(height: 200)

            
                Spacer()
            }

            Spacer()
            
            Button("Set to current location"){
                    self.viewModel.setCurrentLocation()
                }
            .buttonStyle(RoundyButtonStyle()).frame(maxWidth:.infinity)
            
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
    @EnvironmentObject var viewModel: LocationDetailsViewModel
    let place: ClockLocation
    var body: some View {
        HStack {
            Button(place.placeName ?? "Where?") {
                self.viewModel.setLocation(place)
            }.buttonStyle(RoundyButtonStyle()).accentColor(.purple)
            
        }
    }
}

struct LocationDetailsView_Previews: PreviewProvider {
    static let viewModel: LocationDetailsViewModel = {
        let locationService = LocationService()
        locationService.currentLocation = .manchester
        locationService.isConfigured = .configured
        return LocationDetailsViewModel(locationService: locationService)
    }()
    static let clock: Clock = Clock("really", .gold)
    
    static var previews: some View {
        NavigationView(){
            LocationDetailsView().environmentObject(clock)
                .environmentObject(viewModel)
        }
    }
}
