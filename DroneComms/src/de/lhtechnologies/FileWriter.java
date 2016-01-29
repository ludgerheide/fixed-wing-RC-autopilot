package de.lhtechnologies;

import de.lhtechnologies.flightComputer.SerialPortManager;
import de.lhtechnologies.flightComputer.SerialReceiver;

import java.io.*;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Observable;
import java.util.Observer;
import java.util.TimeZone;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 29.01.16.
 */
public class FileWriter implements Observer {
    FileOutputStream fos;

    public FileWriter() throws FileNotFoundException {
        TimeZone tz = TimeZone.getTimeZone("UTC");
        DateFormat df = new SimpleDateFormat("yyyy-MM-dd'T'HH:mmZ");
        df.setTimeZone(tz);
        String nowAsISO = df.format(new Date());

        File f = new File(nowAsISO + ".protobuf");
        fos = new FileOutputStream(f);
    }

    @Override
    public void update(Observable o, Object arg) {
        assert(o instanceof SerialReceiver);
        assert(arg instanceof DroneMessage);

        byte[] messageBytes = ((DroneMessage) arg).toByteArray();
        byte checksum = (byte) SerialPortManager.calculateChecksum(messageBytes.length, messageBytes);

        try {
            fos.write(SerialReceiver.startMarker);
            fos.write(messageBytes);
            fos.write(checksum);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void cleanup() throws Exception {
        FileDescriptor fd = fos.getFD();
        fos.flush();
        fd.sync();
        fos.close();
        System.out.println("File successfully synced!");
    }
}
