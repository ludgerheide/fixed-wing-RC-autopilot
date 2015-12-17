//
//  ProtobufWriter.m
//  ProtoBufToCSV
//
//  Created by Ludger Heide on 24.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "ProtobufWriter.h"
#import <assert.h>

@implementation ProtobufWriter
{
    NSOutputStream *streamInputCommandSet, *streamOutputCommandSet, *streamBatteryData, *streamFlightMode, *streamGPX, *streamPositionVelocity, *streamAttitude, *streamBmpRaw, *streamGyroRaw, *streamMagRaw, *streamAccelRaw;
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
        
        NSString* pathICS = [NSString stringWithFormat: @"%@inputCommands.csv", prefix];
        streamInputCommandSet = [NSOutputStream outputStreamToFileAtPath: pathICS append: NO];
        [streamInputCommandSet scheduleInRunLoop: [NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];
        [streamInputCommandSet open];
        
        NSString* pathOCS = [NSString stringWithFormat: @"%@outputCommands.csv", prefix];
        streamOutputCommandSet = [NSOutputStream outputStreamToFileAtPath: pathOCS append: NO];
        [streamOutputCommandSet scheduleInRunLoop: [NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];
        [streamOutputCommandSet open];
        
        NSString* pathBatteryData = [NSString stringWithFormat: @"%@batteryData.csv", prefix];
        streamBatteryData = [NSOutputStream outputStreamToFileAtPath: pathBatteryData append: NO];
        [streamBatteryData scheduleInRunLoop: [NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];
        [streamBatteryData open];
        
        NSString* pathFlightMode = [NSString stringWithFormat: @"%@flightMode.csv", prefix];
        streamFlightMode = [NSOutputStream outputStreamToFileAtPath: pathFlightMode append: NO];
        [streamFlightMode scheduleInRunLoop: [NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];
        [streamFlightMode open];
        
        NSString* pathGPX = [NSString stringWithFormat: @"%@track.gpx", prefix];
        streamGPX = [NSOutputStream outputStreamToFileAtPath: pathGPX append: NO];
        [streamGPX scheduleInRunLoop: [NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];
        [streamGPX open];
        [self prepareGPXFile];
        
        
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
    if(msg.hasInputCommandSet) {
        NSString* string = [NSString stringWithFormat: @"%u, %u, %u, %u\n", (uint32_t)msg.timestamp, msg.inputCommandSet.yaw, msg.inputCommandSet.pitch, msg.inputCommandSet.thrust];
        NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
        [streamInputCommandSet write: data.bytes maxLength: data.length];
    }
    
    if(msg.hasOutputCommandSet) {
        NSString* string = [NSString stringWithFormat: @"%u, %u, %u, %u\n", (uint32_t)msg.timestamp, msg.outputCommandSet.yaw, msg.outputCommandSet.pitch, msg.outputCommandSet.thrust];
        NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
        [streamOutputCommandSet write: data.bytes maxLength: data.length];
    }
    
    if(msg.hasCurrentBatteryData) {
        NSString* string = [NSString stringWithFormat: @"%u, %u, %u\n", (uint32_t)msg.currentBatteryData.timestamp, msg.currentBatteryData.voltage, msg.currentBatteryData.current];
        NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
        [streamBatteryData write: data.bytes maxLength: data.length];
    }
    
    if(msg.hasCurrentMode) {
        NSString* string = [NSString stringWithFormat: @"%u, %u\n", (uint32_t)msg.timestamp, msg.currentMode];
        NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
        [streamFlightMode write: data.bytes maxLength: data.length];
    }
    
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
        assert(msg.currentPosition.hasRealTime);
        [self writeGPXFile: msg];
        NSString* string = [NSString stringWithFormat: @"%u, %f, %f, %f, %f, %i, %f ,%f\n", (uint32_t)msg.currentPosition.timestamp, msg.currentPosition.realTime, msg.currentPosition.latitude, msg.currentPosition.longitude, msg.currentPosition.gpsAltitude / 100.0, msg.currentPosition.numberOfSatellites, msg.currentSpeed.speed / 1000.0, msg.currentSpeed.courseOverGround / 64.0];
        NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
        [streamPositionVelocity write: data.bytes maxLength: data.length];
    }
    
    if(msg.hasBmpRaw) {
        NSString* string = [NSString stringWithFormat: @"%u, %f, %f, %f\n", (uint32_t)msg.bmpRaw.timestamp, msg.bmpRaw.pressure, msg.bmpRaw.temperature, msg.currentAltitude/100.0];
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
    [streamInputCommandSet close];
    [streamInputCommandSet removeFromRunLoop: [NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamOutputCommandSet close];
    [streamOutputCommandSet removeFromRunLoop: [NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamBatteryData close];
    [streamBatteryData removeFromRunLoop: [NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamFlightMode close];
    [streamFlightMode removeFromRunLoop: [NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamAccelRaw close];
    [streamAccelRaw removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamAttitude close];
    [streamAttitude removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [self closeGPXFIle];
    [streamGPX close];
    [streamGPX removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamPositionVelocity close];
    [streamPositionVelocity removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamBmpRaw close];
    [streamBmpRaw removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamGyroRaw close];
    [streamGyroRaw removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [streamMagRaw close];
    [streamMagRaw removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

-(void) prepareGPXFile {
    NSString* line1 = @"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    NSString* line2 = @"<gpx version=\"1.1\" creator=\"LHT GPXWriter – based on GPSD\"\n";
    NSString* line3 = @"        xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    NSString* line4 = @"        xmlns=\"http://www.topografix.com/GPX/1/1\"\n";
    NSString* line5 = @"        xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1\n";
    NSString* line6 = @"        http://www.topografix.com/GPX/1/1/gpx.xsd\">\n";
    
    //Print the start
    NSString* line7 = @" <trk>\n";
    NSString* line8 = @"  <src>\"LHT GPXWriter – based on GPSD\"</src>\n";
    NSString* line9 = @"  <trkseg>\n";
    
    NSString *string = [NSString stringWithFormat: @"%@%@%@%@%@%@%@%@%@", line1, line2, line3, line4, line5, line6, line7, line8, line9];
    NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
    [streamGPX write: data.bytes maxLength: data.length];
}

-(void) writeGPXFile: (DroneMessage* _Nonnull) msg {
    NSString* line1 = [NSString stringWithFormat: @"    <trkpt lat=\"%f\" lon=\"%f\">\n", msg.currentPosition.latitude, msg.currentPosition.longitude];
    
    NSString* line2 = @"";
    if(msg.hasCurrentAltitude) {
        line2 = [NSString stringWithFormat: @"    <ele>%f</ele>\n", msg.currentAltitude / 100.0];
    }
    
    NSDateComponents *components = [[NSCalendar currentCalendar] components:NSCalendarUnitDay | NSCalendarUnitMonth | NSCalendarUnitYear fromDate:[NSDate date]];
    NSInteger day = [components day];
    NSInteger month = [components month];
    NSInteger year = [components year];
    float gpsTime = msg.currentPosition.realTime;
    unsigned int hour = msg.currentPosition.realTime / 10000;
    unsigned int minute = (gpsTime - (hour * 10000)) / 100;
    float second = gpsTime - (hour * 10000) - (minute * 100);
    NSString* line3 = [NSString stringWithFormat: @"    <time>%04li-%02li-%02liT%02i:%02i:%02fZ</time>\n", (long)year, (long)month, (long)day, hour, minute, second];
    
    NSString* line4 = @"   </trkpt>\n";
    
    NSString *string = [NSString stringWithFormat: @"%@%@%@%@", line1, line2, line3, line4];
    NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
    [streamGPX write: data.bytes maxLength: data.length];
}

-(void) closeGPXFIle {//Print the track end
    NSString* line1 = @"  </trkseg>\n";
    NSString* line2 = @" </trk>\n";
    NSString* line3 = @"</gpx>\n";
    
    NSString *string = [NSString stringWithFormat: @"%@%@%@", line1, line2, line3];
    NSData* data = [string dataUsingEncoding: NSUTF8StringEncoding];
    [streamGPX write: data.bytes maxLength: data.length];
}
@end
