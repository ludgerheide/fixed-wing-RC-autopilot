package de.lhtechnologies;

import org.bouncycastle.openssl.PEMKeyPair;
import org.bouncycastle.openssl.PEMParser;
import org.bouncycastle.openssl.jcajce.JcaPEMKeyConverter;
import org.bouncycastle.util.encoders.Base64;

import java.io.*;
import java.net.URL;
import java.security.*;
import java.security.spec.X509EncodedKeySpec;

/**
 * Created by ludger on 26.01.16.
 */
public class SignVerify {
    private String msgStart = "-----BEGIN MESSAGE-----";
    private String msgEnd = "-----END MESSAGE-----";
    private String sigStart = "-----BEGIN SIGNATURE-----";
    private String sigEnd = "-----END SIGNATURE-----";
    public static String newline = System.getProperty("line.separator");

    private PrivateKey privateSigningKey;
    private PublicKey publicVerificationKey;

    public SignVerify() throws Exception {
        {
            //Import the private key from jar or file system
            String privateKeyName = "drone.priv.pem";
            InputStream privKeyStream;

            try {
                privKeyStream = ClassLoader.getSystemResourceAsStream(privateKeyName);
                if(privKeyStream == null) {
                    throw new Exception();
                }
            } catch (Exception e) {
                privKeyStream = new FileInputStream(privateKeyName);
            }

            BufferedReader br = new BufferedReader(new InputStreamReader(privKeyStream));
            PEMParser pp = new PEMParser(br);
            PEMKeyPair pemKeyPair = (PEMKeyPair) pp.readObject();
            KeyPair kp = new JcaPEMKeyConverter().getKeyPair(pemKeyPair);
            privateSigningKey = kp.getPrivate();
            pp.close();
            br.close();
        }

        {
            //Import the public key (I know this is bad Stackoverflow copypasta)
            String pubKeyName = "controller.pub.pem";
            InputStream pubKeyStream;

            try {
                pubKeyStream = ClassLoader.getSystemResourceAsStream(pubKeyName);
                if(pubKeyStream == null) {
                    throw new Exception();
                }
            } catch (Exception e) {
                pubKeyStream = new FileInputStream(pubKeyName);
            }

            PemFile pemFile = new PemFile(pubKeyStream);
            byte[] content = pemFile.getPemObject().getContent();
            X509EncodedKeySpec pubKeySpec = new X509EncodedKeySpec(content);
            Security.addProvider(new org.bouncycastle.jce.provider.BouncyCastleProvider());
            KeyFactory factory = KeyFactory.getInstance("RSA", "BC");
            publicVerificationKey = factory.generatePublic(pubKeySpec);
        }
    }

    public String createSignedMessage(byte[] message) throws Exception {
        Signature rsa = Signature.getInstance("SHA256withRSA", "SunRsaSign");
        rsa.initSign(privateSigningKey);
        rsa.update(message);
        byte[] signature = rsa.sign();

        String encodedMessage = Base64.toBase64String(message);
        String encodedSignature = Base64.toBase64String(signature);

        return msgStart + newline + encodedMessage + newline + msgEnd + newline + sigStart + newline + encodedSignature + newline + sigEnd;
    }

    public boolean verifyMessage(String message) {
        BufferedReader br = new BufferedReader(new InputStreamReader(new ByteArrayInputStream(message.getBytes())));
        StringBuilder msgBuilder = new StringBuilder();
        StringBuilder sigBuilder = new StringBuilder();

        try {
            boolean inMsg = false;
            for (String line = br.readLine(); line != null; line = br.readLine()) {
                if (!inMsg) {
                    if (line.equals(msgStart)) {
                        inMsg = true;
                    }
                    continue;
                } else {
                    if (line.equals(msgEnd)) {
                        inMsg = false;
                        break;
                    }
                    msgBuilder.append(line);
                }
            }
            boolean inSig = false;
            for (String line = br.readLine(); line != null; line = br.readLine()) {
                if (!inSig) {
                    if (line.equals(sigStart)) {
                        inSig = true;
                    }
                    continue;
                } else {
                    if (line.equals(sigEnd)) {
                        inSig = false;
                        break;
                    }
                    sigBuilder.append(line);
                }
            }

            byte[] msg = Base64.decode(msgBuilder.toString());
            byte[] sig = Base64.decode(sigBuilder.toString());

            Signature rsa = Signature.getInstance("SHA256withRSA", "SunRsaSign");
            rsa.initVerify(publicVerificationKey);
            rsa.update(msg);

            return rsa.verify(sig);
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    public byte[] getMessage(String message) throws Exception {
        BufferedReader br = new BufferedReader(new InputStreamReader(new ByteArrayInputStream(message.getBytes())));
        StringBuilder msgBuilder = new StringBuilder();

        boolean inMsg = false;
        for (String line = br.readLine(); line != null; line = br.readLine()) {
            if (!inMsg) {
                if (line.equals(msgStart)) {
                    inMsg = true;
                }
                continue;
            } else {
                if (line.equals(msgEnd)) {
                    inMsg = false;
                    break;
                }
                msgBuilder.append(line);
            }
        }

        byte[] msg = Base64.decode(msgBuilder.toString());

        if(msg.length == 0) {
            throw new Exception();
        } else {
            return msg;
        }
    }

    public byte[] verifyAndGetMessage(String message) {
        boolean signatureVerified = verifyMessage(message);
        if(signatureVerified) {
            try {
                return getMessage(message);
            } catch (Exception e) {
                e.printStackTrace();
                return null;
            }
        } else {
            return null;
        }
    }
}
