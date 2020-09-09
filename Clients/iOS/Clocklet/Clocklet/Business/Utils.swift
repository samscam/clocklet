//
//  Utils.swift
//  Clocklet
//
//  Created by Sam Easterby-Smith on 05/09/2020.
//  Copyright © 2020 Spotlight Kid Ltd. All rights reserved.
//

import Foundation


enum ConfigState: String {
    case unknown
    case notConfigured
    case configured
    
    static func &&(lhs: ConfigState, rhs: ConfigState)->ConfigState{
        switch (lhs, rhs){
        case (.unknown,_), (_,.unknown):
            return .unknown
        case (.notConfigured,_), (_,.notConfigured):
            return .notConfigured
        case (.configured, .configured):
            return .configured
        }
    }
}

/// This is a (hopefully legit) hack for working out if we are in a debug build
/// Based on:
/// https://blog.wadetregaskis.com/if-debug-in-swift/

func inDebugBuilds<T>(_ code: () -> T) -> T? {
    var isDebug = false
    assert({ isDebug=true; return true }())
    if (isDebug){
        return code()
    }
    return nil
}
