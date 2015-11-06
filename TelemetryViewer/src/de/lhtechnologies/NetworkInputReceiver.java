package de.lhtechnologies;

import java.io.IOException;
import java.net.*;

/**
 * Created by ludger on 06.11.15.
 */
public class NetworkInputReceiver extends InputReceiver {
    public static int DEFAULT_PORT = 5000;

    private DatagramSocket socket;

    public NetworkInputReceiver(String adddressPort) throws Exception {
        // WORKAROUND: add any scheme to make the resulting URI valid.
        URI uri = new URI("my://" + adddressPort); // may throw URISyntaxException
        String host = uri.getHost();
        int port = uri.getPort();

        if(port == -1) {
            port = DEFAULT_PORT;
        }

        if (uri.getHost() == null) {
            throw new URISyntaxException(uri.toString(), "URI must have host part");
        }

        //Now start listening on this connection
        InetAddress address = InetAddress.getByName(host);
        socket = new DatagramSocket(port, address);
    }

    public void startReceiving() {
        while(true) {
            byte[] payloadBuffer = new byte[2048];
            DatagramPacket packet = new DatagramPacket(payloadBuffer, payloadBuffer.length);
            try {
                socket.receive(packet);
            } catch (IOException e) {
                e.printStackTrace();
            }
            //Here, the method blocks until a packet is received
            byte[] payload = new byte[packet.getLength()];
            for(int i = 0; i < payload.length; i++) {
                payload[i] = payloadBuffer[i];
            }
            super.processMessage(payload);
        }
    }
}
