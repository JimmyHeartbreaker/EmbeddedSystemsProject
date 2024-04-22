rng('default')
fs = 32000;                                % sample frequency (Hz)
t = 0:1/fs:10;                      % 1 second span time vector
x = readtable("waveform_no_filter.csv");

%filtered = lowpass(x.Var1,1000,fs);
y = fft(x.Var1);

n = length(y); % number of samples
f = (10:n/2);% frequency range

power = abs(y(10:end/2));

plot(f,power)
xlabel('Frequency')
ylabel('Power')