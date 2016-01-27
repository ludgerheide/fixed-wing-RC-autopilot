package de.lhtechnologies.ircComms;

import org.pircbotx.User;
import org.pircbotx.hooks.Event;
import org.pircbotx.hooks.Listener;
import org.pircbotx.hooks.events.MessageEvent;

import java.util.Observable;

/**
 * Created by ludger on 20.01.16.
 */
public class IrcReceiver extends Observable implements Listener {

    @Override
    public void onEvent(Event event) throws Exception {
        if(event instanceof MessageEvent) {
            String message = ((MessageEvent) event).getMessage();
            User sender = ((MessageEvent) event).getUser();

            setChanged();
            notifyObservers(message);
        }
    }
}
