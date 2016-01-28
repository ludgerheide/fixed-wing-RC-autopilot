package de.lhtechnologies.inetComms;

import de.lhtechnologies.SignVerify;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.time.Duration;
import java.util.Observable;
import java.util.concurrent.*;

/**
 * Created by ludger on 28.01.16.
 */
public class InetReceiver extends Observable implements Runnable {
    private String msgStart = "-----BEGIN MESSAGE-----";
    private String sigEnd = "-----END SIGNATURE-----";
    public static String newline = System.getProperty("line.separator");

    private BufferedReader br;
    private SignVerify sv;

    public InetReceiver(InputStream in, SignVerify sv) {
        br = new BufferedReader(new InputStreamReader(in));
        this.sv = sv;
    }

    @Override
    public void run() {
        while(true) {
            try {
                String startLine = br.readLine();
                if (startLine != null && startLine.equals(msgStart)) {
                    final Duration timeout = Duration.ofSeconds(500);
                    ExecutorService executor = Executors.newSingleThreadExecutor();

                    final Future<String> handler = executor.submit(new Callable() {
                        @Override
                        public String call() throws Exception {
                            return readData();
                        }
                    });

                    try {
                        String rest = handler.get(timeout.toMillis(), TimeUnit.MILLISECONDS);
                        String message = startLine + newline + rest;

                        if (sv.verifyMessage(message)) {
                            byte[] payload = sv.getMessage(message);
                            setChanged();
                            notifyObservers(payload);
                        }
                    } catch (TimeoutException e) {
                        e.printStackTrace();
                        handler.cancel(true);
                    }
                    executor.shutdownNow();
                }
            } catch (Exception e) {
                e.printStackTrace();
                break;
            }
        }
    }

    public String readData() throws IOException {
        StringBuilder outStringBuilder = new StringBuilder();

        //Read a complete message
        for (String line = br.readLine(); line != null; line = br.readLine()) {
            outStringBuilder.append(line);
            outStringBuilder.append(newline);
            if (line.equals(sigEnd)) {
                break;
            }
        }
        return outStringBuilder.toString();
    }
}
