//
//  SwiftyRSA.swift
//  SwiftyRSA
//
//  Created by Loïs Di Qual on 7/2/15.
//  Copyright (c) 2015 Scoop Technologies, Inc. All rights reserved.
//

import Foundation
import Security

open class SwiftyRSAError: NSError {
    init(message: String) {
        super.init(domain: "com.takescoop.SwiftyRSA", code: 500, userInfo: [
            NSLocalizedDescriptionKey: message
        ])
    }

    @available(*, unavailable)
    required public init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

open class SwiftyRSA {
    
    fileprivate var keyTags: [Data] = []
    fileprivate static let defaultPadding: SecPadding = .PKCS1
    
    // MARK: - Public Advanced Methods
    
    public init() {}
    
    open func publicKeyFromDERData(_ keyData: Data) throws -> SecKey {
        return try addKey(keyData, isPublic: true)
    }
    
    open func publicKeyFromPEMString(_ key: String) throws -> SecKey {
        let data = try dataFromPEMKey(key)
        return try addKey(data, isPublic: true)
    }
    
    open func privateKeyFromPEMString(_ key: String) throws -> SecKey {
        let data = try dataFromPEMKey(key)
        return try addKey(data, isPublic: false)
    }
    
    // MARK: - Private
    
    fileprivate func addKey(_ keyData: Data, isPublic: Bool) throws -> SecKey {
        
        var keyData = keyData
        
        // Strip key header if necessary
        if isPublic {
            try keyData = stripPublicKeyHeader(keyData)
        }
        
        let tag = UUID().uuidString
        let tagData = Data(bytes: UnsafePointer<UInt8>(tag), count: tag.lengthOfBytes(using: String.Encoding.utf8))
        removeKeyWithTagData(tagData)
        
        // Add persistent version of the key to system keychain
        //let persistKey = 0 TODO: is it really not needed?
        let keyClass   = isPublic ? kSecAttrKeyClassPublic : kSecAttrKeyClassPrivate
        
        // Add persistent version of the key to system keychain
        let keyDict = NSMutableDictionary()
        keyDict.setObject(kSecClassKey,         forKey: kSecClass as! NSCopying)
        keyDict.setObject(tagData,              forKey: kSecAttrApplicationTag as! NSCopying)
        keyDict.setObject(kSecAttrKeyTypeRSA,   forKey: kSecAttrKeyType as! NSCopying)
        keyDict.setObject(keyData,              forKey: kSecValueData as! NSCopying)
        keyDict.setObject(keyClass,             forKey: kSecAttrKeyClass as! NSCopying)
        keyDict.setObject(NSNumber(value: true as Bool), forKey: kSecReturnPersistentRef as! NSCopying)
        keyDict.setObject(kSecAttrAccessibleWhenUnlocked, forKey: kSecAttrAccessible as! NSCopying)
        
        var secStatus = SecItemAdd(keyDict as CFDictionary, nil)
        if secStatus != noErr && secStatus != errSecDuplicateItem {
            throw SwiftyRSAError(message: "Provided key couldn't be added to the keychain")
        }
        
        keyTags.append(tagData)
        
        // Now fetch the SecKeyRef version of the key
        var keyRef: AnyObject? = nil
        keyDict.removeObject(forKey: kSecValueData)
        keyDict.removeObject(forKey: kSecReturnPersistentRef)
        keyDict.setObject(NSNumber(value: true as Bool), forKey: kSecReturnRef as! NSCopying)
        keyDict.setObject(kSecAttrKeyTypeRSA,   forKey: kSecAttrKeyType as! NSCopying)
        secStatus = SecItemCopyMatching(keyDict as CFDictionary, &keyRef)
        
        guard let unwrappedKeyRef = keyRef else {
            throw SwiftyRSAError(message: "Couldn't get key reference from the keychain")
        }
        
        return unwrappedKeyRef as! SecKey
    }
    
    fileprivate func dataFromPEMKey(_ key: String) throws -> Data {
        let rawLines = key.components(separatedBy: "\n")
        var lines = [String]()
        
        for line in rawLines {
            if line == "-----BEGIN RSA PRIVATE KEY-----" ||
                line == "-----END RSA PRIVATE KEY-----"   ||
                line == "-----BEGIN PUBLIC KEY-----" ||
                line == "-----END PUBLIC KEY-----"   ||
                line == "" {
                    continue
            }
            lines.append(line)
        }
        
        if lines.count == 0 {
            throw SwiftyRSAError(message: "Couldn't get data from PEM key: no data available after stripping headers")
        }
        
        // Decode base64 key
        let base64EncodedKey = lines.joined(separator: "")
        let keyData = Data(base64Encoded: base64EncodedKey, options: .ignoreUnknownCharacters)
        
        guard let unwrappedKeyData = keyData, unwrappedKeyData.count != 0 else {
            throw SwiftyRSAError(message: "Couldn't decode PEM key data (base64)")
        }
        
        return unwrappedKeyData
    }
    
    fileprivate func stripPublicKeyHeader(_ keyData: Data) throws -> Data {
        let count = keyData.count / MemoryLayout<CUnsignedChar>.size
        var byteArray = [CUnsignedChar](repeating: 0, count: count)
        (keyData as NSData).getBytes(&byteArray, length: keyData.count)
        
        var index = 0
        if byteArray[index] != 0x30 {
            throw SwiftyRSAError(message: "Invalid byte at index 0 (\(byteArray[0])) for public key header")
        }
        index += 1
        
        if byteArray[index] > 0x80 {
            index += Int(byteArray[index]) - 0x80 + 1
        }
        else {
            index += 1
        }
        
        let seqiod: [CUnsignedChar] = [0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
            0x01, 0x05, 0x00]
        byteArray.replaceSubrange(CountableRange<Int>(index ..< index + seqiod.count), with: seqiod)
        
        index += 15
        
        if byteArray[index] != 0x03 {
            throw SwiftyRSAError(message: "Invalid byte at index \(index - 1) (\(byteArray[index - 1])) for public key header")
        }
        index += 1
        
        if byteArray[index] > 0x80 {
            index += Int(byteArray[index]) - 0x80 + 1
        }
        else {
            index += 1
        }
        
        if byteArray[index] != 0 {
            throw SwiftyRSAError(message: "Invalid byte at index \(index - 1) (\(byteArray[index - 1])) for public key header")
        }
        index += 1
        
        let test = [CUnsignedChar](byteArray[index...keyData.count - 1])
        
        let data = Data(bytes: UnsafePointer<UInt8>(test), count: keyData.count - index)
        
        return data
    }
    
    fileprivate func removeKeyWithTagData(_ tagData: Data) {
        let publicKey = NSMutableDictionary()
        publicKey.setObject(kSecClassKey,       forKey: kSecClass as! NSCopying)
        publicKey.setObject(kSecAttrKeyTypeRSA, forKey: kSecAttrKeyType as! NSCopying)
        publicKey.setObject(tagData,            forKey: kSecAttrApplicationTag as! NSCopying)
        SecItemDelete(publicKey as CFDictionary)
    }
    
    deinit {
        for tagData in keyTags {
            removeKeyWithTagData(tagData)
        }
    }
}
