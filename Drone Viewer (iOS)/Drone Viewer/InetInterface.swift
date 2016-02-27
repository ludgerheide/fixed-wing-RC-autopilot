//
//  InetInterface.swift
//  tcpClient
//
//  Created by Ludger Heide on 30.01.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import Foundation
import UIKit //TODO: REMOVE

class InetInterface : NSObject, NSStreamDelegate {
    private let host = "192.168.15.10"
    private let port: UInt32 = 5051
    private var reconnectTimer: NSTimer?
    private let reconnecTimeout: NSTimeInterval = 1
    
    private let msgStart = "-----BEGIN MESSAGE-----"
    private let sigEnd = "-----END SIGNATURE-----"
    private let maxReadLength = 1024
    private var incompleteMessage: String = ""
    
    private let sv: SignVerify
    internal static let notificationName = "NewDroneMessageNotification"
    
    private var inStream: NSInputStream?
    private var outStream: NSOutputStream?
    
    override init() {
        Logger.log(" interface")
        sv = SignVerify.init()!
        super.init()
    }
    
    internal func startNetworkCommunication() {
        
        var readStream: Unmanaged<CFReadStreamRef>?
        var writeStream: Unmanaged<CFWriteStreamRef>?
        let hostAsCFStringRef = host as CFStringRef
        
        //If we are already connected, try to close it
        closeNetwork()
        
        CFStreamCreatePairWithSocketToHost(nil, hostAsCFStringRef, port, &readStream, &writeStream)
        
        inStream = readStream!.takeRetainedValue()
        outStream = writeStream!.takeRetainedValue()
        
        inStream!.delegate = self
        outStream!.delegate = self
        
        inStream!.scheduleInRunLoop(NSRunLoop.currentRunLoop(), forMode: NSDefaultRunLoopMode)
        outStream!.scheduleInRunLoop(NSRunLoop.currentRunLoop(), forMode: NSDefaultRunLoopMode)
        
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
        incompleteMessage.removeAll(keepCapacity: true)
    }
    
    @objc internal func stream(aStream: NSStream, handleEvent eventCode: NSStreamEvent) {
        switch eventCode {
        case NSStreamEvent.OpenCompleted:
            Logger.log("Stream opened!")
        case NSStreamEvent.HasBytesAvailable:
            Logger.log("Stream has bytes available!")
            assert(aStream == inStream)
            inStreamHasBytesAvaliableHandler()
        case NSStreamEvent.HasSpaceAvailable:
            Logger.log("Stream has space available!")
            assert(aStream == outStream)
            outStreamHasSpaceHandler(eventCode)
        case NSStreamEvent.ErrorOccurred, NSStreamEvent.EndEncountered:
            Logger.log("Error or end ocurred")
            //Do this stuff in the main thread only
            reconnectTimer?.invalidate()
            reconnectTimer = nil
            reconnectTimer = NSTimer.scheduledTimerWithTimeInterval(reconnecTimeout, target: self, selector: "startNetworkCommunication", userInfo: nil, repeats: false)
        default:
            Logger.log("Unexpected stuff happened!")
            startNetworkCommunication()
        }
    }
    
    private func inStreamHasBytesAvaliableHandler() {
        //Read our message from the stream. If it matches a start sequnce, start storing it until we get an end sequence or the read times out
        let inData = NSMutableData(capacity: maxReadLength)
        let inBuffer = UnsafeMutablePointer<UInt8>(inData!.mutableBytes)
        let readSize = inStream!.read(inBuffer, maxLength: maxReadLength)
        let inBytes = NSData(bytes: inBuffer, length: readSize)
        let inString = String(data: inBytes, encoding: NSASCIIStringEncoding)!
        
        incompleteMessage += inString
        if let receivedMessage = handleMessage() {
            let (verificationResult, payload) = sv.verifyMessage(receivedMessage)
            if(verificationResult == true && payload != nil) {
                let error: NSErrorPointer = nil
                let droneMessage: DroneMessage = DroneMessage.init(data: payload!, error: error)
                
                if(error == nil) {
                    let notificationCenter = NSNotificationCenter.defaultCenter()
                    let theNotification: NSNotification = NSNotification.init(name: InetInterface.notificationName, object: droneMessage)
                    notificationCenter.postNotification(theNotification)
                } else {
                    Logger.log("Error decoding DroneMessage!")
                }
            } else {
                Logger.log("Error validating signature!")
            }
            
            
        }
    }
    
    func sendMessage(message: DroneMessage!) {
        //Create a signed message
        let payload = message.data()
        if(payload != nil) {
            do {
                let signedMessage = try sv.createSignedMessage(payload!)
                let buf = [UInt8](signedMessage.utf8)
                outStream?.write(buf, maxLength: buf.count)
            } catch {
                Logger.log("Error signing message!")
            }
        } else {
            Logger.log("Error creating payload!")
        }
        
    }
    
    private func handleMessage() -> String? {
        if let startRange = incompleteMessage.rangeOfString(msgStart) {
            let startMark = startRange.startIndex
            
            if let endRange = incompleteMessage.rangeOfString(sigEnd) {
                let endMark = endRange.endIndex
                
                let completeRange = Range(start: startMark, end: endMark)
                let outString = incompleteMessage.substringWithRange(completeRange)
                
                let rangeToRemove = Range(start: startMark, end: endMark)
                incompleteMessage.removeRange(rangeToRemove)
                
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
        incompleteMessage.removeAll(keepCapacity: true)
        return nil
    }
    
    private func outStreamHasSpaceHandler(eventCode: NSStreamEvent) {
        Logger.log("Outstream has space")
    }
}