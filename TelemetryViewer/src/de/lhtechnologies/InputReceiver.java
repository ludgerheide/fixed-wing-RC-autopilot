package de.lhtechnologies;

import com.google.protobuf.InvalidProtocolBufferException;
import org.LiveGraph.dataFile.write.DataStreamWriter;
import org.LiveGraph.dataFile.write.DataStreamWriterFactory;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.UnknownHostException;

import static de.lhtechnologies.CommunicationProtocol.*;

/**
 * Created by ludger on 06.11.15.
 */
public abstract class InputReceiver {
    public static String startMarker = "start";

    public FileOutputStream outFile;
    public DatagramSocket outSocket;
    public String outAddress;
    public int outPort;
    public boolean sync;

    public boolean verbose;
    private int validMessageCount;

    //Variables for the live graphing
    public boolean dataStreamEnabled;
    private DataStreamWriter accelWriter;
    public static String ACCEL_FILENAME = "accel.dat";
    public static String ACCEL_RAW_TIME = "accelRawT";
    public static String ACCEL_RAW_X = "accelRawX";
    public static String ACCEL_RAW_Y = "accelRawY";
    public static String ACCEL_RAW_Z = "accelRawZ";

    private DataStreamWriter gyroWriter;
    public static String GYRO_FILENAME = "gyro.dat";
    public static String GYRO_RAW_TIME = "gyroRawT";
    public static String GYRO_RAW_X = "gyroRawX";
    public static String GYRO_RAW_Y = "gyroRawY";
    public static String GYRO_RAW_Z = "gyroRawZ";

    private DataStreamWriter magWriter;
    public static String MAG_FILENAME = "mag.dat";
    public static String MAG_RAW_TIME = "magRawT";
    public static String MAG_RAW_X = "magRawX";
    public static String MAG_RAW_Y = "magRawY";
    public static String MAG_RAW_Z = "magRawZ";

    private DataStreamWriter attitudeWriter;
    public static String ATTITUDE_FILENAME = "attitude.dat";
    public static String ATTITUDE_TIME = "attitudeT";
    public static String ATTITUDE_COURSE = "attitudeCourse";
    public static String ATTITUDE_PITCH = "attitudePitch";
    public static String ATTITUDE_ROLL = "attitudeRoll";

    private DataStreamWriter bmpWriter;
    public static String BMP_FILENAME = "bmp.dat";
    public static String BMP_TIME = "time";
    public static String BMP_TEMPERATURE = "temperature";
    public static String BMP_PRESSURE = "pressure";
    public static String BMP_ALTITUDE = "altitude";

    private DataStreamWriter batteryWriter;
    public static String BATTERY_FILENAME = "battery.dat";
    public static String BATTERY_TIME = "time";
    public static String BATTERY_VOLTAGE = "voltage";
    public static String BATTERY_CURRENT = "current";

    public abstract void startReceiving();

    public void processMessage(byte[] protobuf) throws InvalidProtocolBufferException {
        DroneMessage msg;

        msg = DroneMessage.parseFrom(protobuf);

        if(outFile != null) {
            //Write start to the ouput file
            try {
                outFile.write(startMarker.getBytes());

                //Create the length
                byte[] lengthByte = new byte[1];
                lengthByte[0] = (byte)protobuf.length;
                outFile.write(lengthByte);
                outFile.write(protobuf);
                outFile.write(InputReceiver.calculateChecksum(protobuf));
                outFile.flush();
                if(sync) {
                    outFile.getFD().sync();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        if(outSocket != null) {
            //Send out over network
            InetAddress address = null;
            try {
                address = InetAddress.getByName(outAddress);
            } catch (UnknownHostException e) {
                e.printStackTrace();
            }
            DatagramPacket packet = new DatagramPacket(protobuf, protobuf.length, address, outPort);
            try {
                outSocket.send(packet);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        if(accelWriter != null) {
            writeDataStream(msg);
        }

        //Write to display
        System.out.format("Message %d received%n", validMessageCount++);
        if(verbose) {
            System.out.println(msg);
        }
    }

    public static byte[] calculateChecksum (byte[] input) {
        long checksum = 0;
        for(int i = 0; i < input.length; i++) {
            checksum += input[i] & 0xFF; //To see the unsigned value
        }
        byte[] out = new byte[1];
        out[0] = (byte)checksum;
        return out;
    }

    public void setupDataStream() {
        //Setup the data series
        File accelFile = new File(ACCEL_FILENAME);
        accelWriter = DataStreamWriterFactory.createDataWriter(accelFile, true);
        accelWriter.addDataSeries(ACCEL_RAW_TIME);
        accelWriter.addDataSeries(ACCEL_RAW_X);
        accelWriter.addDataSeries(ACCEL_RAW_Y);
        accelWriter.addDataSeries(ACCEL_RAW_Z);

        File gyroFile = new File(GYRO_FILENAME);
        gyroWriter = DataStreamWriterFactory.createDataWriter(gyroFile, true);
        gyroWriter.addDataSeries(GYRO_RAW_TIME);
        gyroWriter.addDataSeries(GYRO_RAW_X);
        gyroWriter.addDataSeries(GYRO_RAW_Y);
        gyroWriter.addDataSeries(GYRO_RAW_Z);

        File magFile = new File(MAG_FILENAME);
        magWriter = DataStreamWriterFactory.createDataWriter(magFile, true);
        magWriter.addDataSeries(MAG_RAW_TIME);
        magWriter.addDataSeries(MAG_RAW_X);
        magWriter.addDataSeries(MAG_RAW_Y);
        magWriter.addDataSeries(MAG_RAW_Z);

        File attitudeFile = new File(ATTITUDE_FILENAME);
        attitudeWriter = DataStreamWriterFactory.createDataWriter(attitudeFile, true);
        attitudeWriter.addDataSeries(ATTITUDE_TIME);
        attitudeWriter.addDataSeries(ATTITUDE_COURSE);
        attitudeWriter.addDataSeries(ATTITUDE_PITCH);
        attitudeWriter.addDataSeries(ATTITUDE_ROLL);

        File bmpFile = new File(BMP_FILENAME);
        bmpWriter = DataStreamWriterFactory.createDataWriter(bmpFile, true);
        bmpWriter.addDataSeries(BMP_TIME);
        bmpWriter.addDataSeries(BMP_TEMPERATURE);
        bmpWriter.addDataSeries(BMP_PRESSURE);
        bmpWriter.addDataSeries(BMP_ALTITUDE);

        File batteryFile = new File(BATTERY_FILENAME);
        batteryWriter = DataStreamWriterFactory.createDataWriter(batteryFile, true);
        batteryWriter.addDataSeries(BATTERY_TIME);
        batteryWriter.addDataSeries(BATTERY_VOLTAGE);
        batteryWriter.addDataSeries(BATTERY_CURRENT);
    }

    public void writeDataStream(DroneMessage msg) {
        if(msg.hasAccelRaw()) {
            accelWriter.setDataValue(msg.getAccelRaw().getTimestamp());
            accelWriter.setDataValue(msg.getAccelRaw().getX());
            accelWriter.setDataValue(msg.getAccelRaw().getY());
            accelWriter.setDataValue(msg.getAccelRaw().getZ());
            accelWriter.writeDataSet();
        }

        if(msg.hasGyroRaw()) {
            gyroWriter.setDataValue(msg.getGyroRaw().getTimestamp());
            gyroWriter.setDataValue(msg.getGyroRaw().getX());
            gyroWriter.setDataValue(msg.getGyroRaw().getY());
            gyroWriter.setDataValue(msg.getGyroRaw().getZ());
            gyroWriter.writeDataSet();
        }

        if(msg.hasMagRaw()) {
            magWriter.setDataValue(msg.getMagRaw().getTimestamp());
            magWriter.setDataValue(msg.getMagRaw().getX());
            magWriter.setDataValue(msg.getMagRaw().getY());
            magWriter.setDataValue(msg.getMagRaw().getZ());
            magWriter.writeDataSet();
        }

        if(msg.hasCurrentAttitude()) {
            attitudeWriter.setDataValue(msg.getCurrentAttitude().getTimestamp());
            attitudeWriter.setDataValue(msg.getCurrentAttitude().getCourseMagnetic() / 64.0);
            attitudeWriter.setDataValue(msg.getCurrentAttitude().getPitch() / 64.0);
            attitudeWriter.setDataValue(msg.getCurrentAttitude().getRoll() / 64.0);
            attitudeWriter.writeDataSet();
        }

        if(msg.hasBmpRaw() && msg.hasSeaLevelPressure()) {
            bmpWriter.setDataValue(msg.getBmpRaw().getTimestamp());
            bmpWriter.setDataValue(msg.getBmpRaw().getTemperature());
            bmpWriter.setDataValue(msg.getBmpRaw().getPressure());

            double seaLevelPressure = msg.getSeaLevelPressure();
            double currentPressure = msg.getBmpRaw().getPressure();
            double altitude = 44330.0 * (1.0 - Math.pow(currentPressure / seaLevelPressure, 0.1903));
            bmpWriter.setDataValue(altitude);
            bmpWriter.writeDataSet();
        }

        if(msg.hasCurrentBatteryData()) {
            batteryWriter.setDataValue(msg.getCurrentBatteryData().getTimestamp());
            batteryWriter.setDataValue(msg.getCurrentBatteryData().getVoltage());
            batteryWriter.setDataValue(msg.getCurrentBatteryData().getCurrent());
            batteryWriter.writeDataSet();
        }
    }
}
