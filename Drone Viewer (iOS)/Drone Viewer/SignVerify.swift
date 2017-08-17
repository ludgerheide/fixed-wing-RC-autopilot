//
//  SignVerify.swift
//  sigTest
//
//  Created by Ludger Heide on 26.01.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

import Foundation
import Security

enum SignVerifyError: Error {
    case signingFailed
    case verifyingFailed
}

class SignVerify {
    fileprivate let msgStart = "-----BEGIN MESSAGE-----";
    fileprivate let msgEnd = "-----END MESSAGE-----";
    fileprivate let sigStart = "-----BEGIN SIGNATURE-----";
    fileprivate let sigEnd = "-----END SIGNATURE-----";
    
    fileprivate let privateSigningKey: SecKey!
    fileprivate let publicVerificationKey: SecKey!
    
    func createSignedMessage(_ message: Data!) throws -> String! {
        //Create the SHA256 hash of the data
        let digest = NSMutableData(length: Int(CC_SHA256_DIGEST_LENGTH))!
        let buffer = digest.mutableBytes.bindMemory(to: UInt8.self, capacity: message.count)
        message.withUnsafeBytes {(bytes: UnsafePointer<UInt8>)->Void in
            CC_SHA256(bytes, CC_LONG(message.count), buffer)
        }
        //Now sign the hash
        let signature = NSMutableData(length: SecKeyGetBlockSize(privateSigningKey))!
        var signatureLength = signature.length
        
        let err: OSStatus = SecKeyRawSign(privateSigningKey,
            SecPadding.PKCS1SHA256,
            digest.bytes.bindMemory(to: UInt8.self, capacity: digest.length),
            digest.length,
            signature.mutableBytes.bindMemory(to: UInt8.self, capacity: signature.length),
            &signatureLength)
        
        if(err != errSecSuccess) {
            Logger.log("Error during signing!")
            throw SignVerifyError.signingFailed
        }
        
        return msgStart + "\n" + message.base64EncodedString(options: NSData.Base64EncodingOptions(rawValue: 0)) + "\n" + msgEnd + "\n" + sigStart + "\n" + signature.base64EncodedString(options: NSData.Base64EncodingOptions(rawValue: 0)) + "\n" + sigEnd + "\n"
    }
    
    func verifyMessage(_ message: String) -> (verified: Bool?, content: Data?) {
        let array = message.components(separatedBy: "\n")
        
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
        let decodedContent = Data(base64Encoded: content.data(using: String.Encoding.ascii)!, options: NSData.Base64DecodingOptions(rawValue: 0))!
        let decodedSignature = Data(base64Encoded: signature.data(using: String.Encoding.ascii)!, options: NSData.Base64DecodingOptions(rawValue: 0))!
        let decodedSignatureLength = decodedSignature.count
        
        //Now verify the signature
        
        //Create the SHA256 hash of the data
        let digest = NSMutableData(length: Int(CC_SHA256_DIGEST_LENGTH))!
        CC_SHA256((decodedContent as NSData).bytes, CC_LONG(decodedContent.count), digest.mutableBytes.bindMemory(to: UInt8.self, capacity: decodedContent.count))
        
        let status = SecKeyRawVerify(publicVerificationKey,
            SecPadding.PKCS1SHA256,
            digest.bytes.bindMemory(to: UInt8.self, capacity: digest.length),
            digest.length,
            UnsafeMutablePointer<UInt8>(mutating: (decodedSignature as NSData).bytes.bindMemory(to: UInt8.self, capacity: decodedSignature.count)),
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
            let pubKeyPath = Bundle.main.path(forResource: "drone.pub.pem", ofType: nil)
            let pubKeyString = try String(contentsOfFile: pubKeyPath!)
            publicVerificationKey = try rsa.publicKeyFromPEMString(pubKeyString)
        } catch {
            privateSigningKey = nil
            publicVerificationKey = nil
            Logger.log("Error creating public key ref!")
            return nil
        }
        
        do{
            //Import the private key
            let privKeyPath = Bundle.main.path(forResource: "controller.priv.pem", ofType: nil)
            let privKeyString = try String(contentsOfFile: privKeyPath!)
            privateSigningKey = try rsa.privateKeyFromPEMString(privKeyString)
        } catch {
            privateSigningKey = nil
            Logger.log("Error creating private key ref!")
            return nil
        }
    }
}
