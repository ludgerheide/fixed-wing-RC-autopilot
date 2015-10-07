/**
 * Copyright (c) 2014-2015 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */
package de.lhtechnologies;

import com.digi.xbee.api.listeners.IDataReceiveListener;
import com.digi.xbee.api.models.XBeeMessage;
import com.digi.xbee.api.utils.HexUtils;

import javax.swing.*;

/**
 * Class to manage the XBee received data that was sent by other modules in the 
 * same network.
 *
 * <p>Acts as a data listener by implementing the 
 * {@link IDataReceiveListener} interface, and is notified when new 
 * data for the module is received.</p>
 *
 * @see IDataReceiveListener
 *
 */
public class MyDataReceiveListener implements IDataReceiveListener,Runnable {

    private JPanel lPanel;
    private JPanel rPanel;
    private JPanel subPan;
    private JPanel centerPanel;
    private JPanel upPan;
    private boolean readThreadAlive = false;

    Thread readThread;

    public MyDataReceiveListener(JPanel a, JPanel b, JPanel c, JPanel d, JPanel e) {
        this.readThread = new Thread(this);
        this.lPanel = a;
        this.rPanel = b;
        this.subPan = c;
        this.centerPanel = d;
        this.upPan = e;

        try{
            readThread.start();
            readThreadAlive = true;
        } catch(IllegalThreadStateException evtTh){
            System.out.println("Problems with Threads");
        }
    }

    /*
     * (non-Javadoc)
     * @see com.digi.xbee.api.listeners.IDataReceiveListener#dataReceived(com.digi.xbee.api.models.XBeeMessage)
     */
    @Override
    public void dataReceived(XBeeMessage xbeeMessage) {
        byte[] inData = xbeeMessage.getData();

        try {
            CommunicationProtocol.DroneMessage myMsg = CommunicationProtocol.DroneMessage.parseFrom(inData);

            float yaw = myMsg.getCurrentAttitude().getCourseMagnetic() / 64;
            float pitch = myMsg.getCurrentAttitude().getPitch() / 64;
            float roll = myMsg.getCurrentAttitude().getRoll() / 64;

            ArtificialHorizon.yawValueFiltered = (int)yaw;
            ArtificialHorizon.pitchValueFiltered = (int)pitch;
            ArtificialHorizon.rollValueFiltered = (int)roll;

            lPanel.repaint();
            rPanel.repaint();
            subPan.repaint();
            centerPanel.repaint();
            upPan.repaint();

            System.out.format("Magnetic course: %3.2f, Pitch: %3.2f, Roll: %3.2f %n", yaw, pitch, roll);
        } catch (Exception e) {
            e.printStackTrace();
        }



        System.out.format("From %s >> %s | %n", xbeeMessage.getDevice().get64BitAddress(),
                HexUtils.prettyHexString(HexUtils.byteArrayToHexString(xbeeMessage.getData())));
    }

    @Override
    public void run() {
        while(readThreadAlive) {
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}