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


struct LocationDetailsView: View {
    
    @EnvironmentObject var viewModel: LocationDetailsViewModel
    @EnvironmentObject var clock: Clock
     
    static let popularLocations: [ClockLocation] = [
        .manchester,.newYork,.london,.paris,.munich,.sanfrancisco,.melbourne,.chatham,.mumbai,.helipark,.northPole,.southPole,.nullIsland
    ]
    
    
    var body: some View {
        ScrollView{
        VStack(alignment: .leading){
            if viewModel.showMap {
            
                ZStack{
                    
                    // Some flip-flop here - tried using SwiftUI's Map - but it produces nasty runtime errors
                    MapView(coordinate: viewModel.currentLocation.coordinate)
                    
                    viewModel.currentLocation.placeName.map{ placeName in
                        VStack(alignment: .center){
                        HStack(alignment:.top){
                            Text(placeName).font(.largeTitle).fontWeight(.black).multilineTextAlignment(.center).shadow(color: Color(UIColor.systemBackground), radius:5)
                        }.padding(5)
                        Spacer()
                        }.padding(5)
                    }
                }.frame(height: 200)
                    .cornerRadius(20)

            
                Spacer()
            }

            Spacer()
            Button {
                viewModel.setCurrentLocation()
            } label: {
                Text("Set to current location").frame(maxWidth:.infinity)
            }.buttonStyle(RoundyButtonStyle())

            
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
                self.viewModel.setLocation(self.place)
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
