package de.lhtechnologies.ircComms;

import de.lhtechnologies.SignVerify;
import org.pircbotx.PircBotX;
import org.pircbotx.output.OutputIRC;

/**
 * Created by ludger on 20.01.16.
 */
public class IrcTransmitter {
    public PircBotX bot;
    SignVerify sv;

    public IrcTransmitter(PircBotX bot) {
        //Initialize the Signging/Verifying stuff
        try {
            sv = new SignVerify();
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(-1);
        }

        this.bot = bot;
    }

    public void transmit(byte[] message) {
        if(bot.isConnected()) {
            try {
                String outMsg = sv.createSignedMessage(message);
                OutputIRC out = new OutputIRC(bot);
                out.message("regdul", outMsg);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public void transmitString(String message) {
        if(bot.isConnected()) {
            OutputIRC out = new OutputIRC(bot);
            out.message(IrcManager.channel, message);
        }
    }
}
