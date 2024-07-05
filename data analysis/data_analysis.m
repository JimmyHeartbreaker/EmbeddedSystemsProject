
raw_wave = readtable("waveform_4x_sample_raw.csv");
plot(raw_wave.Var1);
title("raw");
figure
hd = getFilter2;
y = filter(hd,raw_wave.Var1);
plot(y);
title("IIR Filtered");
figure
filtered_wave = readtable("waveform_500hz_iirFiltered.csv");
plot(filtered_wave.Var1);
title("IIR 500Hz Filtered");
figure

n = length(filtered_wave.Var1);
fs = 100000     ;
fftFilt = fft(filtered_wave.Var1);
f = fs/n*(0:n-1);

plot(f, (abs(fftFilt)));
title("fft filtered");
xlabel('freq'); ylabel('Amp (dB)'); grid on
xlim([0 20000]);
figure;

%analog filter
cutoff = 1000;
theta = [pi/8,3*pi/8];
pole = -sin(theta) + cos(theta)*1i;
pole = pole * 2 * pi * cutoff
stage1 = tf(abs(pole(1))^2,[1,-2*real(pole(1)),abs(pole(1))^2]);
stage2 = tf(abs(pole(2))^2,[1,-2*real(pole(2)),abs(pole(2))^2]);

bode(stage1*stage2)

    perCycle = 200/100000;
    fiveCycles = 3*perCycle;
    
    t = linspace(0,fiveCycles,600);
    
    lsim(sys2plus3,filtered_wave.Var1,t)



