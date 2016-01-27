//
//  SignVerify.swift
//  sigTest
//
//  Created by Ludger Heide on 26.01.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import Foundation
import Security

enum SignVerifyError: ErrorType {
    case SigningFailed
    case VerifyingFailed
}

class SignVerify {
    private let msgStart = "-----BEGIN MESSAGE-----";
    private let msgEnd = "-----END MESSAGE-----";
    private let sigStart = "-----BEGIN SIGNATURE-----";
    private let sigEnd = "-----END SIGNATURE-----";
    
    private let privateSigningKey: SecKeyRef!
    private let publicVerificationKey: SecKeyRef!
    
    func createSignedMessage(message: NSData!) throws -> String! {
        //Create the SHA256 hash of the data
        let digest = NSMutableData(length: Int(CC_SHA256_DIGEST_LENGTH))!
        CC_SHA256(message.bytes, CC_LONG(message.length), UnsafeMutablePointer<UInt8>(digest.mutableBytes))
        
        //Now sign the hash
        let signature = NSMutableData(length: SecKeyGetBlockSize(privateSigningKey))!
        var signatureLength = signature.length
        
        let err: OSStatus = SecKeyRawSign(privateSigningKey,
            SecPadding.PKCS1SHA256,
            UnsafePointer<UInt8>(digest.bytes),
            digest.length,
            UnsafeMutablePointer<UInt8>(signature.mutableBytes),
            &signatureLength)
        
        if(err != errSecSuccess) {
            print("Error during signing!")
            throw SignVerifyError.SigningFailed
        }
        
        return msgStart + "\n" + message.base64EncodedStringWithOptions(NSDataBase64EncodingOptions(rawValue: 0)) + "\n" + msgEnd + "\n" + sigStart + "\n" + signature.base64EncodedStringWithOptions(NSDataBase64EncodingOptions(rawValue: 0)) + "\n" + sigEnd;
    }
    
    func verifyMessage(message: String) -> (verified: Bool!, content: NSData?) {
        let array = message.componentsSeparatedByString("\n")
        
        var content = ""
        var signature = ""
        var inMsg = false
        var inSig = false
        
        for s in array {
            if (s == msgStart) {
                inMsg = true
                continue
            }
            if (s == msgEnd) {
                inMsg = false
                continue
            }
            if (inMsg == true) {
                content += s
            }
            
            if (s == sigStart) {
                inSig = true
                continue
            }
            if (s == sigEnd) {
                inSig = false
                continue
            }
            if (inSig == true) {
                signature += s
            }
        }
        
        //Now, we should have a content and signature
        if(content.isEmpty || signature.isEmpty) {
            return (false, nil)
        }
        
        //Decode the content and sig
        let decodedContent = NSData(base64EncodedData: content.dataUsingEncoding(NSASCIIStringEncoding)!, options: NSDataBase64DecodingOptions(rawValue: 0))!
        let decodedSignature = NSData(base64EncodedData: signature.dataUsingEncoding(NSASCIIStringEncoding)!, options: NSDataBase64DecodingOptions(rawValue: 0))!
        let decodedSignatureLength = decodedSignature.length
        
        //Now verify the signature
        
        //Create the SHA256 hash of the data
        let digest = NSMutableData(length: Int(CC_SHA256_DIGEST_LENGTH))!
        CC_SHA256(decodedContent.bytes, CC_LONG(decodedContent.length), UnsafeMutablePointer<UInt8>(digest.mutableBytes))
        
        let status = SecKeyRawVerify(publicVerificationKey,
            SecPadding.PKCS1SHA256,
            UnsafePointer<UInt8>(digest.bytes),
            digest.length,
            UnsafeMutablePointer<UInt8>(decodedSignature.bytes),
            decodedSignatureLength)
        
        if (status != errSecSuccess) {
            return (false, nil)
        } else {
            return (true, decodedContent)
        }
    }
    
    init?() {
        let rsa : SwiftyRSA = SwiftyRSA()
        
        do {
            //Import the public key
            let pubKeyPath = NSBundle.mainBundle().pathForResource("drone.pub.pem", ofType: nil)
            let pubKeyString = try String(contentsOfFile: pubKeyPath!)
            publicVerificationKey = try rsa.publicKeyFromPEMString(pubKeyString)
        } catch {
            privateSigningKey = nil
            publicVerificationKey = nil
            print("Error creating public key ref!")
            return nil
        }
        
        do{
            //Import the private key
            let privKeyPath = NSBundle.mainBundle().pathForResource("controller.priv.pem", ofType: nil)
            let privKeyString = try String(contentsOfFile: privKeyPath!)
            privateSigningKey = try rsa.privateKeyFromPEMString(privKeyString)
        } catch {
            privateSigningKey = nil
            print("Error creating private key ref!")
            return nil
        }
    }
}