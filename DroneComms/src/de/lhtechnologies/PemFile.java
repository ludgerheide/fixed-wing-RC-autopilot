package de.lhtechnologies;

import org.bouncycastle.util.io.pem.PemObject;
import org.bouncycastle.util.io.pem.PemReader;

import java.io.*;

/**
 * Created by ludger on 26.01.16.
 */
public class PemFile {

    private PemObject pemObject;

    public PemFile(String filename) throws FileNotFoundException, IOException {
        PemReader pemReader = new PemReader(new InputStreamReader(
                new FileInputStream(filename)));
        try {
            this.pemObject = pemReader.readPemObject();
        } finally {
            pemReader.close();
        }
    }

    public PemFile(InputStream in) throws IOException {
        PemReader pemReader = new PemReader(new InputStreamReader(in));
        try {
            this.pemObject = pemReader.readPemObject();
        } finally {
            pemReader.close();
        }
    }

    public PemObject getPemObject() {
        return pemObject;
    }
}
