package de.lhtechnologies;

import com.digi.xbee.api.XBeeDevice;
import com.digi.xbee.api.exceptions.XBeeException;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

/**
 * XBee Java Library Receive Data sample application.
 *
 * <p>This example registers a listener to manage the received data.</p>
 *
 * <p>For a complete description on the example, refer to the 'ReadMe.txt' file
 * included in the root directory.</p>
 */
public class ArtificialHorizon {
    /***************************************************************
     * Those instances variables store the value collected via USART
     **************************************************************/
    static int pitchValuePure = 0;
    static int rollValuePure = 0;

    static int pitchValueFiltered = 0;
    static int rollValueFiltered = 0;

    static int yawValueFiltered = 0;
    static int yawValuePure = 0;

	/* Constants */

    // TODO Replace with the serial port where your receiver module is connected.
    private static final String PORT = "/dev/tty.usbserial-DA017KGT";
    // TODO Replace with the baud rate of you receiver module.
    private static final int BAUD_RATE = 115200;

    /**
     * Application main method.
     *
     * @param args Command line arguments.
     */
    public static void main(String[] args) {
        Runnable myRunner = new Runnable(){

            @Override
            public void run() {

                final float VERSION = 2.5f;
                Dimension dimScreenSize = Toolkit.getDefaultToolkit().getScreenSize();
                int xLocation;
                int yLocation;


                /**************************
                 * Definition of the frame
                 * for the main application
                 *************************/
                // Create the side panels
                // As a default the left is or kalman and the right side is from the sensors
                PanelGUI leftPanel = new PanelGUI("kalman");
                PanelGUI rightPanel = new PanelGUI("sensor");

                // Create the sub panel
                SubPanel subPanel = new SubPanel();

                // Create the center panel
                CenterPanel centerPanel = new CenterPanel();

                // Create the upper panel
                UpPanel upPanel = new UpPanel();

                System.out.println(" +-----------------------------------------+");
                System.out.println(" |  XBee Java Library Receive Data Sample  |");
                System.out.println(" +-----------------------------------------+\n");

                XBeeDevice myDevice = new XBeeDevice(PORT, BAUD_RATE);

                try {
                    myDevice.open();

                    myDevice.addDataListener(new MyDataReceiveListener(leftPanel, rightPanel, subPanel, centerPanel, upPanel));

                    System.out.println("\n>> Waiting for data...");

                } catch (XBeeException e) {
                    e.printStackTrace();
                    System.exit(1);
                }

                /*************************
                 * Create here the menuBar
                 ************************/
                JMenuBar menuBar = new JMenuBar();

                /*************************************
                 * Create a Box for spacing JButton
                 ************************************/
                menuBar.add(Box.createHorizontalStrut(300));


                /**********************
                 * Create a JButton for
                 * closing the program
                 *********************/
                JButton closeApp = new JButton("Exit");
                menuBar.add(closeApp);

                ActionListener closeProg = new ActionListener(){
                    @Override
                    public void actionPerformed(ActionEvent evtProg) {
                        System.exit(0);
                    }
                };
                closeApp.addActionListener(closeProg);


                /*******************************
                 * Create a JButton for
                 * information about the license
                 ******************************/
                JButton about = new JButton("About");
                menuBar.add(about);

                ActionListener aboutLic = new ActionListener(){
                    @Override
                    public void actionPerformed(ActionEvent arg0) {
                        JOptionPane info = new JOptionPane("Artificial Horizon AH-3+  ver: " + VERSION + "\n\nCopyright (C) 2010 Davide Picchi" + "\n<paveway@gmail.com>" + "\n" + "\nLicensed under GNU GPL v3", JOptionPane.INFORMATION_MESSAGE);
                        JDialog dialog = info.createDialog("License");
                        dialog.setVisible(true);
                    }

                };
                about.addActionListener(aboutLic);


                /*********************
                 * Create the main GUI
                 *********************/
                JFrame mainFrame = new JFrame("Artificial Horizon  Ver: " + VERSION);
                mainFrame.setUndecorated(true);
                mainFrame.getRootPane().setWindowDecorationStyle(JRootPane.FRAME);
                mainFrame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
                mainFrame.setResizable(true);
                mainFrame.setLayout(new BorderLayout());

                mainFrame.getContentPane().add(BorderLayout.WEST, leftPanel);
                mainFrame.getContentPane().add(BorderLayout.EAST, rightPanel);
                mainFrame.getContentPane().add(BorderLayout.SOUTH, subPanel);
                mainFrame.getContentPane().add(BorderLayout.CENTER, centerPanel);
                mainFrame.getContentPane().add(BorderLayout.NORTH, upPanel);
                mainFrame.pack();

                xLocation = (dimScreenSize.width - mainFrame.getWidth()) / 2;
                yLocation = (dimScreenSize.height - mainFrame.getHeight()) / 2;
                mainFrame.setLocation(xLocation, yLocation);


                mainFrame.addWindowListener(new WindowAdapter(){

                    public void windowClosing(WindowEvent winEvt){
                        int answer;
                        answer = JOptionPane.showConfirmDialog(winEvt.getWindow(), "Exit the application?", "Please select:", JOptionPane.YES_NO_OPTION);
                        if(answer == JOptionPane.YES_OPTION){
                            winEvt.getWindow().dispose();
                            System.exit(0);
                        }
                    }
                });

                mainFrame.setJMenuBar(menuBar);

                mainFrame.setVisible(true);
            }


        };

        EventQueue.invokeLater(myRunner);
    }
}