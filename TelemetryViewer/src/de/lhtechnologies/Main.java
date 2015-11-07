package de.lhtechnologies;

import com.beust.jcommander.JCommander;
import com.sun.javaws.exceptions.InvalidArgumentException;
import sun.nio.ch.Net;

import java.io.FileOutputStream;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.URI;
import java.net.URISyntaxException;

public class Main {

    public static void main(String[] args) throws Exception {
        CommandLineParser clp = new CommandLineParser();
        new JCommander(clp, args);

        //Input parsing: Fail if we want to receive from both network and serial
        if (clp.receiveAddress != null && clp.deviceFile != null) {
            throw new Exception("This program cannot receive from both a device file and network source!");
        }

        //Also fail if we have neither a network nor file source
        if (clp.receiveAddress == null && clp.deviceFile == null) {
            throw new Exception("No data source set!");
        }

        //Create our input receiver
        InputReceiver inputReceiver;
        if(clp.receiveAddress != null) {
            inputReceiver = new NetworkInputReceiver(clp.receiveAddress);
        } else {
            inputReceiver = new SerialInputReceiver(clp.deviceFile);
        }

        //Create our file writer (if applicapble)
        if(clp.outputFile != null) {
            FileOutputStream fo = new FileOutputStream(clp.outputFile);
            inputReceiver.outFile = fo;
        }

        //Create out network sender (if applicapble)
        if(clp.sendAddress != null) {
            // WORKAROUND: add any scheme to make the resulting URI valid.
            URI uri = new URI("my://" + clp.sendAddress); // may throw URISyntaxException
            String host = uri.getHost();
            int port = uri.getPort();

            if(port == -1) {
                port = NetworkInputReceiver.DEFAULT_PORT;
            }
            inputReceiver.outPort = port;

            if (uri.getHost() == null) {
                throw new URISyntaxException(uri.toString(), "URI must have host part");
            }
            inputReceiver.outAddress = uri.getHost();

            //Now start listening on this connection
            InetAddress address = InetAddress.getByName("0.0.0.0");
            inputReceiver.outSocket = new DatagramSocket(NetworkInputReceiver.DEFAULT_PORT, address);
        }

        //Set verbosity
        inputReceiver.verbose = clp.verbose;

        //Set syncinc
        inputReceiver.sync = clp.sync;

        //Receive data in an infinite loop
        inputReceiver.startReceiving();
    }
}
