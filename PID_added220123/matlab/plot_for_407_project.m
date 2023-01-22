%%on off 1
figure()
plot(onoff1.VarName1, onoff1.VarName2)
hold on
plot(onoff1.VarName1, onoff1.VarName3)
grid minor
xlabel('DateTime')
ylabel('Current (A)')
title('ON-OFF Control (1) of the Armature Current')

%%on off 2
figure()
plot(onoff2.VarName1, onoff2.VarName2)
hold on
plot(onoff2.VarName1, onoff2.VarName3)
grid minor
xlabel('DateTime')
ylabel('Current (A)')
title('ON-OFF Control (2) of the Armature Current')

%pi 1
a = 1;
b = 4532;
figure()
plot(pi1.VarName1(a:b), pi1.VarName2(a:b), LineWidth=2)
hold on
plot(pi1.VarName1(a:b), pi1.VarName3(a:b))
grid minor
xlabel('DateTime')
ylabel('Current (A)')
title('PI Control (1) of the Armature Current')

%pi 2              lowpass(pi2.VarName3(a:end), 1/5000, 1/100)
a = 1;
b = 10000;
figure()
plot(pi2.VarName1(a:end), pi2.VarName2(a:end), LineWidth=2)
hold on
plot(pi2.VarName1(a:end), pi2.VarName3(a:end))
grid minor
xlabel('DateTime')
ylabel('Current (A)')
title('PI Control of the Armature Current')