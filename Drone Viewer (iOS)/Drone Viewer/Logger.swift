//
//  Logger.swift
//  Drone Viewer
//
//  Created by Ludger Heide on 04.02.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

class Logger {
    static func log(content: String, functionName: String = __FUNCTION__, fileName: String = __FILE__, lineNumber: Int = __LINE__) {
        print(fileName + ":" + String(lineNumber) + "(" + functionName + ") " + content)
    }
}
