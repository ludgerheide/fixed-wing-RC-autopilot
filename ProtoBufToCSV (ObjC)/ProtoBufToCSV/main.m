//
//  main.m
//  ProtoBufToCSV
//
//  Created by Ludger Heide on 24.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CommunicationProtocol.pbobjc.h"
#import "ProtobufWriter.h"

uint8_t startSequence[5] = {'s', 't', 'a', 'r', 't'};

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSString* path = [NSString stringWithCString: argv[1] encoding: NSASCIIStringEncoding];
        NSInputStream* iStream = [NSInputStream inputStreamWithFileAtPath: path];
        [iStream scheduleInRunLoop:[NSRunLoop currentRunLoop]
                           forMode:NSDefaultRunLoopMode];
        [iStream open];
        
        unsigned long checksumErrors = 0;
        unsigned long decodingErrors = 0;
        unsigned long successCount = 0;
        
        ProtobufWriter* writer = [[ProtobufWriter alloc] init];
        
        while ([iStream hasBytesAvailable]) {
            //Search until the magic sequence is found
            uint8_t startByte;
            for(uint8_t i = 0; i < sizeof(startSequence); i++) {
                [iStream read: &startByte maxLength: 1];
                if(startByte != startSequence[i]) {
                    break;
                }
            }
            uint8_t length;
            [iStream read: &length maxLength: 1];
            
            uint8_t* msgBuf = malloc(length);
            [iStream read: msgBuf maxLength: length];
            
            uint8 checksum;
            [iStream read: &checksum maxLength: 1];
            
            uint8_t calculatedChecksum = 0;
            for(uint8_t i = 0; i < length; i++) {
                calculatedChecksum += msgBuf[i];
            }
            @autoreleasepool {
                if(checksum == calculatedChecksum) {
                    NSData* theData = [NSData dataWithBytes: msgBuf length: length];
                    NSError* myError;
                    DroneMessage* msg = [[DroneMessage alloc] initWithData: theData error: &myError];
                    if(myError) {
                        decodingErrors++;
                    } else {
                        [writer writeMessage: msg];
                        successCount++;
                    }
                } else {
                    checksumErrors++;
                }
                
            }
            free(msgBuf);
        }
        NSLog(@"Decoding Errors:  %9lu", decodingErrors);
        NSLog(@"Checksum errors:  %9lu", checksumErrors);
        NSLog(@"Written messages: %9lu", successCount);
        [writer flush];
    }
    return 0;
}
