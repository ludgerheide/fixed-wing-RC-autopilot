//
//  ProtobufWriter.h
//  ProtoBufToCSV
//
//  Created by Ludger Heide on 24.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CommunicationProtocol.pbobjc.h"

@interface ProtobufWriter : NSObject

-(id _Nullable) initWithPrefix: (NSString* _Nullable) prefix;
-(void) writeMessage: (DroneMessage* _Nonnull) msg;
-(void) flush;
@end
