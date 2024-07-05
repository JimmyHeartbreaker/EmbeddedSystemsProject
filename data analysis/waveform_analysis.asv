% 
% % 
fc = 1000;
fs = 32000;

[b,a] = butter(4,0.0625);
fprintf('%s\n',sprintf(' %.20f,',b))
 % 
  fprintf('%s\n',sprintf(' %.20f,',a))

freqz(b,a,[],fs);
%subplot(3,1,1);
ylim([-100 20]);
title('filter');
figure
x = readtable("waveform_no_filter.csv");
plot(x.Var1(1:1000)/4);
title('waveform unfiltered');
figure;
dataIn = x.Var1/4;%randn(1000,1);
dataOut = filter(b,a,dataIn);

plot(dataOut);
title('filtered');
figure

y = fft(dataOut);

n = length(y); % number of samples
f = (10:n/2);% frequency range

power = abs(y(10:end/2));

plot(f,power)
title('fft filtered');
xlabel('Frequency')
ylabel('Power')
figure


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
title('fft unfiltered');
xlabel('Frequency')
ylabel('Power')

 % 
 % 
 % 
 % fprintf('%s\n',sprintf(' %.15f,',b))
 % 
 % fprintf('%s\n',sprintf(' %.15f,',a))
 % [h,f] = freqz(b,1,[],Fs);
 % plot(f,mag2db(abs(h)))
 % xlabel('Frequency (Hz)')
 % ylabel('Magnitude (dB)')
 % grid

% 
% 
% 
% fs = 32000;
% [A,B,C,D] = butter(10,[50 1000]/(fs/2))
% %sos = ss2sos(A,B,C,D)
% %freqz(sos,[],fs)


      % 1 second span time vector
% noisetable = readtable("waveform128.csv");
% guiartable = readtable("waveform.csv");
% signal = guiartable.Var1(1:1000);
% noise = noisetable.Var1(1001:2000);
% blms = dsp.BlockLMSFilter(10,5);
%  blms.StepSize = 0.01;
%  blms.WeightsOutputPort = false;
%  filt = dsp.FIRFilter;
%  filt.Numerator = fir1(10,[.5, .75]);
%  x = filt(noise); % Noise
 %x = randn(1000,1);
% signal =  sin(0:.05:49.95)'
 % d = signal; % Noise + Signal
 % [y, err] = blms(x, d);
 % subplot(3,1,1);
 % plot(d);
 % title('Noise + Signal');
 % subplot(3,1,2);
 % plot(err);
 % title('Signal');
 % subplot(3,1,3);
 % plot(noise);
 % title('noise');