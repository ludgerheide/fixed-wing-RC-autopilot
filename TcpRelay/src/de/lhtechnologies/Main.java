package de.lhtechnologies;

public class Main {
    private static int port1 = 5050;
    private static int port2 = 5051;

    public static void main(String[] args) {
        TCPRelay relay1, relay2;

        try {
            relay1 = new TCPRelay(port1);
            relay2 = new TCPRelay(port2);

            relay1.setCounterpart(relay2);
            relay2.setCounterpart(relay1);

            new Thread(relay1).start();
            new Thread(relay2).start();
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(-1);
        }
    }
}
