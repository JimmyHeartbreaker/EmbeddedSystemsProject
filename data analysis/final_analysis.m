Fs = 32000;
n = 255;
Wn = [0.0625];
b = fir1(n,Wn);
freqz(b,[1.0],[],Fs);
ylim([-100 20]);
figure
wave = readtable("waveform_4x_sample_raw.csv");
plot(wave.Var1);
title("raw");
hd = getFilter2;
y = filter(hd,wave.Var1);
plot(y);
title("IIR Filtered");

hold on

dataOut1 = filter(b,1,wave.Var1);
plot(dataOut1);
title("FIR filtered")


% dataOut2 = filter([0.5],[1.0,-0.5],dataOut1);
% plot(dataOut2);
% title("IIR filtered")
% hold off

sys = tf(1,[1,1.41422,1]);
bode(sys);
grid;

Hd=c2d(sys,0.5,'tustin')


sys = tf(6283/3.948e7,[1 12566 3.948e7]);
bode(sys)