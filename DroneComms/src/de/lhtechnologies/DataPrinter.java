package de.lhtechnologies;

import java.util.Observable;
import java.util.Observer;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 18.01.16.
 */
public class DataPrinter implements Observer {
    @Override
    public void update(Observable o, Object arg) {
        assert(arg instanceof DroneMessage);
        DroneMessage msg = (DroneMessage) arg;
        System.out.println(msg);
    }
}
