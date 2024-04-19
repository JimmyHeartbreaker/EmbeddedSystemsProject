% rng('default')
% fs = 32000;                                % sample frequency (Hz)
% t = 0:1/fs:10;                      % 1 second span time vector
% x = readtable("waveform.csv");
% %filtered = lowpass(x.Var1,1000,fs);
% y = fft(x.Var1);
% y = fftshift(y);% DFT of signal
% 
% n = length(y); % number of samples
% %f = (10:n);% frequency range
% fs = 32000;
% f = (9:n-1)*(fs/n);
% power = abs(y(10:end));
% 
% plot(f,power)
% xlabel('Frequency')
% ylabel('Power')

n = 15;
Wn = 0.4;
b = fir1(n,Wn)