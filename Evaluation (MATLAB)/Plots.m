inPitchDegrees = (inputCommands.pitch - 127) .* (20/127);

numPlots = 5;

subplot(numPlots,1,1);
plot(inputCommands.timestamp, inPitchDegrees, attitude.timestamp, attitude.pitch);
title('Pitch: E/A');
legend('in', 'out');

subplot(numPlots,1,2);
plot(baroData.timestamp, baroData.altitude);
title('Flughöhe');

subplot(numPlots,1,3);
plot(flightMode.timestamp, flightMode.flightMode);
title('FlightMode Autonomous = 3, degraded = 0, flybywire = 2');

subplot(numPlots,1,4);
plot(positionVelocity.timestamp, positionVelocity.speed)
title('GPS-Geschwindigkeit');

subplot(numPlots,1,5);
plot(positionVelocity.timestamp, positionVelocity.satelliteCount)
title('Anzahl Satelliten');