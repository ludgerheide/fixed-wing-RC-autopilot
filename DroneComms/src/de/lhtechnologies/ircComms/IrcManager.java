package de.lhtechnologies.ircComms;

import org.pircbotx.Configuration;
import org.pircbotx.PircBotX;
import org.pircbotx.exception.IrcException;

import java.io.IOException;

/**
 * Created by ludger on 20.01.16.
 */

public class IrcManager implements Runnable {
    private static String hostname = "irc.freenode.net";
    private static String name = "planePi";
    public static String channel = "#test";

    private PircBotX myBot;
    public IrcReceiver receiver;
    public IrcTransmitter transmitter;

    public IrcManager() {
        receiver = new IrcReceiver();

        Configuration<PircBotX> config = new Configuration.Builder<PircBotX>()
                .setServerHostname(hostname)
                .setName(name)
                .addAutoJoinChannel(channel)
                .setAutoReconnect(true)
                .addListener(receiver)
                .buildConfiguration();

        myBot = new PircBotX(config);

        transmitter = new IrcTransmitter(myBot);
    }

    @Override
    public void run() {
        while (true) {
            try {
                myBot.startBot();
            } catch (IOException e) {
                e.printStackTrace();
            } catch (IrcException e) {
                e.printStackTrace();
            }
        }
    }
}
