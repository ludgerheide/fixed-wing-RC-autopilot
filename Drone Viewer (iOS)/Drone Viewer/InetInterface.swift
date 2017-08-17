//
//  InetInterface.swift
//  tcpClient
//
//  Created by Ludger Heide on 30.01.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import Foundation
import UIKit //TODO: REMOVE

class InetInterface : NSObject, StreamDelegate {
    fileprivate let host = "lht.no-ip.biz"
    fileprivate let port: UInt32 = 5051
    fileprivate var reconnectTimer: Timer?
    fileprivate let reconnecTimeout: TimeInterval = 1
    
    fileprivate let msgStart = "-----BEGIN MESSAGE-----"
    fileprivate let sigEnd = "-----END SIGNATURE-----"
    fileprivate let maxReadLength = 1024
    fileprivate var incompleteMessage: String = ""
    
    fileprivate let sv: SignVerify
    internal static let notificationName = "NewDroneMessageNotification"
    internal static let statusNotificationName = "statusNotification"
    
    fileprivate var inStream: InputStream?
    fileprivate var outStream: OutputStream?
    
    override init() {
        Logger.log(" interface")
        sv = SignVerify.init()!
        super.init()
    }
    
    internal func startNetworkCommunication() {
        
        var readStream: Unmanaged<CFReadStream>?
        var writeStream: Unmanaged<CFWriteStream>?
        let hostAsCFStringRef = host as CFString
        
        //If we are already connected, try to close it
        closeNetwork()
        
        CFStreamCreatePairWithSocketToHost(nil, hostAsCFStringRef, port, &readStream, &writeStream)
        
        inStream = readStream!.takeRetainedValue()
        outStream = writeStream!.takeRetainedValue()
        
        inStream!.delegate = self
        outStream!.delegate = self
        
        inStream!.schedule(in: RunLoop.current, forMode: RunLoopMode.defaultRunLoopMode)
        outStream!.schedule(in: RunLoop.current, forMode: RunLoopMode.defaultRunLoopMode)
        
        inStream!.open()
        outStream!.open()
    }
    
    internal func closeNetwork() {
        if let localInStream = inStream {
            localInStream.close()
            localInStream.delegate = nil
            Logger.log("Closing inStream")
        }
        inStream = nil
        
        if let localOutStream = outStream {
            localOutStream.close()
            localOutStream.delegate = nil
            Logger.log("Closing outStream")
        }
        outStream = nil
        incompleteMessage.removeAll(keepingCapacity: true)
    }
    
    @objc internal func stream(_ aStream: Stream, handle eventCode: Stream.Event) {
        switch eventCode {
        case Stream.Event.openCompleted:
            Logger.log("Stream opened!")
        case Stream.Event.hasBytesAvailable:
            Logger.log("Stream has bytes available!")
            assert(aStream == inStream)
            inStreamHasBytesAvaliableHandler()
        case Stream.Event.hasSpaceAvailable:
            Logger.log("Stream has space available!")
            assert(aStream == outStream)
            outStreamHasSpaceHandler(eventCode)
        case Stream.Event.errorOccurred, Stream.Event.endEncountered:
            Logger.log("Error or end ocurred")
            closeNetwork()
            //Do this stuff in the main thread only
            reconnectTimer?.invalidate()
            reconnectTimer = nil
            reconnectTimer = Timer.scheduledTimer(timeInterval: reconnecTimeout, target: self, selector: #selector(InetInterface.startNetworkCommunication), userInfo: nil, repeats: false)
            
            //If this strem is the output stream, send an error notification
            if(aStream == outStream) {
                let notificationCenter = NotificationCenter.default
                let theNotification: Notification = Notification.init(name: Notification.Name(rawValue: InetInterface.statusNotificationName), object: eventCode.rawValue)
                notificationCenter.post(theNotification)
            }
            
        default:
            Logger.log("Unexpected stuff happened!")
            startNetworkCommunication()
        }
    }
    
    fileprivate func inStreamHasBytesAvaliableHandler() {
        //Read our message from the stream. If it matches a start sequnce, start storing it until we get an end sequence or the read times out
        let inData = NSMutableData(capacity: maxReadLength)
        let inBuffer = inData!.mutableBytes.bindMemory(to: UInt8.self, capacity: inData!.length)
        let readSize = inStream!.read(inBuffer, maxLength: maxReadLength)
        let inBytes = Data(bytes: UnsafePointer<UInt8>(inBuffer), count: readSize)
        let inString = String(data: inBytes, encoding: String.Encoding.ascii)!
        
        incompleteMessage += inString
        if let receivedMessage = handleMessage() {
            let (verificationResult, payload) = sv.verifyMessage(receivedMessage)
            if(verificationResult == true && payload != nil) {
                do {
                let droneMessage: DroneMessage = try DroneMessage.init(data: payload!)
                    let notificationCenter = NotificationCenter.default
                    let theNotification: Notification = Notification.init(name: Notification.Name(rawValue: InetInterface.notificationName), object: droneMessage)
                    notificationCenter.post(theNotification)
                } catch {
                    Logger.log("Error decoding DroneMessage!")
                }
            } else {
                Logger.log("Error validating signature!")
            }
            
            
        }
    }
    
    func sendMessage(_ message: DroneMessage!) {
        if(outStream == nil) {
            let notificationCenter = NotificationCenter.default
            let theNotification: Notification = Notification.init(name: Notification.Name(rawValue: InetInterface.statusNotificationName), object: Stream.Event.errorOccurred.rawValue)
            notificationCenter.post(theNotification)
        } else {
            
            //Create a signed message
            let payload = message.data()
            if(payload != nil) {
                do {
                    let signedMessage = try sv.createSignedMessage(payload!)
                    let buf = [UInt8](signedMessage!.utf8)
                    outStream!.write(buf, maxLength: buf.count)
                } catch {
                    Logger.log("Error signing message!")
                }
            } else {
                Logger.log("Error creating payload!")
            }
        }
    }
    
    fileprivate func handleMessage() -> String? {
        if let startRange = incompleteMessage.range(of: msgStart) {
            let startMark = startRange.lowerBound
            
            if let endRange = incompleteMessage.range(of: sigEnd) {
                let endMark = endRange.upperBound
                
                let completeRange = Range(startMark ..< endMark)
                let outString = incompleteMessage.substring(with: completeRange)
                
                let rangeToRemove = Range(startMark ..< endMark)
                incompleteMessage.removeSubrange(rangeToRemove)
                
                let messageFromRecursion: String? = handleMessage()
                
                if(messageFromRecursion == nil) {
                    return outString
                } else {
                    return messageFromRecursion!
                }
            } else {
                return nil
            }
        }
        incompleteMessage.removeAll(keepingCapacity: true)
        return nil
    }
    
    fileprivate func outStreamHasSpaceHandler(_ eventCode: Stream.Event) {
        Logger.log("Outstream has space available!")
        
        let notificationCenter = NotificationCenter.default
        let theNotification: Notification = Notification.init(name: Notification.Name(rawValue: InetInterface.statusNotificationName), object: eventCode.rawValue)
        notificationCenter.post(theNotification)
    }
}
