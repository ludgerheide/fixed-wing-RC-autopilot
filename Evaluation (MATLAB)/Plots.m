subplot(3,1,1)
plot(inputCommands.timestamp, inputCommands.yaw, outputCommands.timestamp, outputCommands.yaw);
legend('in','out');
title('yaw');

subplot(3,1,2)
plot(inputCommands.timestamp, inputCommands.pitch, outputCommands.timestamp, outputCommands.pitch);
legend('in','out');
title('pitch');

subplot(3,1,3)
plot(inputCommands.timestamp, inputCommands.thrust, outputCommands.timestamp, outputCommands.thrust);
legend('in','out');
title('thrust');