//
//  Clock.swift
//  Clockses
//
//  Created by Sam Easterby-Smith on 24/09/2019.
//  Copyright © 2019 Spotlight Kid Ltd. All rights reserved.
//

import Foundation
import SwiftUI

struct Clock: Identifiable {
    let id: UUID
    var serial: UInt32
    var name: String
    var caseColour: CaseColour
}

enum CaseColour: String, Codable {
    case bare
    case black
    case wood
    
    var imageName: String {
        switch self{
        case .bare: return "esp32feather"
        case .black: return "black-clocklet"
        case .wood: return "wood-clocklet"
        }
    }
}
