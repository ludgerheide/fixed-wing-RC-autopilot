//
//  ProtobufWriter.m
//  ProtoBufToCSV
//
//  Created by Ludger Heide on 24.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "ProtobufWriter.h"

@implementation ProtobufWriter
{
    NSOutputStream *streamPositionVelocity, *streamAttitude, *streamBmpRaw, *streamGyroRaw, *streamMagRaw, *streamAccelRaw;
}

-(id) init {
    return [self initWithPrefix: nil];
}

-(id) initWithPrefix: (NSString* _Nullable) prefix {
    self = [super init];
    if(self) {
        if(prefix == nil) {
            prefix = @"";
        }
        NSString* pathAccelRaw = [NSString stringWithFormat: @"%@accelRaw.csv", prefix];
        streamAccelRaw = [NSOutputStream outputStreamToFileAtPath: pathAccelRaw append: NO];
        [streamAccelRaw scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [streamAccelRaw open];
        
        NSString* pathAttitude = [NSString stringWithFormat: @"%@attitude.csv", prefix];
        streamAttitude = [NSOutputStream outputStreamToFileAtPath: pathAttitude append: NO];
        [streamAttitude scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [streamAttitude open];
        
        NSString* pathPosVel = [NSString stringWithFormat: @"%@positionVelocity.csv", prefix];
        streamPositionVelocity = [NSOutputStream outputStreamToFileAtPath: pathPosVel append: NO];
        [streamPositionVelocity scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [streamPositionVelocity open];
        
        NSString* pathBmpRaw = [NSString stringWithFormat: @"%@baroData.csv", prefix];
        streamBmpRaw = [NSOutputStream outputStreamToFileAtPath: pathBmpRaw append: NO];
        [streamBmpRaw scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [streamBmpRaw open];
        
        NSString* pathGyroRaw = [NSString stringWithFormat: @"%@gyroRaw.csv", prefix];
        streamGyroRaw = [NSOutputStream outputStreamToFileAtPath: pathGyroRaw append: NO];
        [streamGyroRaw scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [streamGyroRaw open];
        
        NSString* pathMagRaw = [NSString stringWithFormat: @"%@magRaw.csv", prefix];
        streamMagRaw = [NSOutputStream outputStreamToFileAtPath: pathMagRaw append: NO];
        [streamMagRaw scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [streamMagRaw open];
    }
    return self;
}

-(void) writeMessage: (DroneMessage* _Nonnull) msg {
    if(msg.hasAccelRaw) {
        NSString* string = [NSString stringWithFormat: @"%u, %f, %f, %f\n", (uint32_t)msg.accelRaw.timestamp, msg.accelRaw.x, msg.accelRaw.y, msg.accelRaw.z];
        NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
        [streamAccelRaw write: data.bytes maxLength: data.length];
    }
    
    if(msg.hasCurrentAttitude) {
        NSString* string = [NSString stringWithFormat: @"%u, %f, %f, %f\n", (uint32_t)msg.currentAttitude.timestamp, msg.currentAttitude.courseMagnetic/64.0, msg.currentAttitude.pitch/64.0, msg.currentAttitude.roll/64.0];
        NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
        [streamAttitude write: data.bytes maxLength: data.length];
    }
    
    if(msg.hasCurrentPosition) {
        assert(msg.hasCurrentSpeed);
        NSString* string = [NSString stringWithFormat: @"%u, %f, %f, %f, %f, %f\n", (uint32_t)msg.currentPosition.timestamp, msg.currentPosition.latitude, msg.currentPosition.longitude, msg.currentPosition.altitude / 100.0, msg.currentSpeed.speed / 1000.0, msg.currentSpeed.courseOverGround / 64.0];
        NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
        [streamPositionVelocity write: data.bytes maxLength: data.length];
    }
    
    if(msg.hasBmpRaw) {
        NSString* string = [NSString stringWithFormat: @"%u, %f, %f\n", (uint32_t)msg.bmpRaw.timestamp, msg.bmpRaw.pressure, msg.bmpRaw.temperature];
        NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
        [streamBmpRaw write: data.bytes maxLength: data.length];
    }
    
    if(msg.hasGyroRaw) {
        NSString* string = [NSString stringWithFormat: @"%u ,%f, %f, %f\n", (uint32_t)msg.gyroRaw.timestamp, msg.gyroRaw.x, msg.gyroRaw.y, msg.gyroRaw.z];
        NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
        [streamGyroRaw write: data.bytes maxLength: data.length];
    }
    
    if(msg.hasMagRaw) {
        NSString* string = [NSString stringWithFormat: @"%u ,%f, %f, %f\n", (uint32_t)msg.magRaw.timestamp, msg.magRaw.x, msg.magRaw.y, msg.magRaw.z];
        NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
        [streamMagRaw write: data.bytes maxLength: data.length];
    }
}

-(void) flush {
    [streamAccelRaw close];
    [streamAccelRaw removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamAttitude close];
    [streamAttitude removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamPositionVelocity close];
    [streamPositionVelocity removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamBmpRaw close];
    [streamBmpRaw removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamGyroRaw close];
    [streamGyroRaw removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamMagRaw close];
    [streamMagRaw removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

@end
