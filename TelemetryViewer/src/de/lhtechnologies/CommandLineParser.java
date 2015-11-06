package de.lhtechnologies;

import com.beust.jcommander.Parameter;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Stream;

/**
 * Created by ludger on 06.11.15.
 */
public class CommandLineParser {
    @Parameter
    private List<String> parameters = new ArrayList<>();

    @Parameter(names = { "-v", "--verbose" }, description = "Level of verbosity")
    public boolean verbose = false;

    @Parameter(names = {"-o", "--output"}, description = "Output file to write the protobuf to")
    public String outputFile;

    @Parameter(names = {"-s", "--send"}, description = "IP address and port (x.x.x.x:yyyy) to send the output to")
    public String sendAddress;

    @Parameter(names = {"-r", "--receive"}, description = "IP address and port (x.x.x.x:yyyy) to receive the output from (mutually exclusive with -d)")
    public String receiveAddress;

    @Parameter(names = {"-d", "--device"}, description = "Device to receive serial data from (mutually exclusive with -r)")
    public String deviceFile;
}
