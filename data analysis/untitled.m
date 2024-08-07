
noisetable = readtable("waveform128.csv");
%guitartable = readtable("waveform.csv");
blms = dsp.BlockLMSFilter(10,100);
 blms.StepSize = 0.0001;
 blms.WeightsOutputPort = false;
 filt = dsp.FIRFilter;
 filt.Numerator = fir1(10,[.5, .75]);
% x = randn(1000,1); % Noise
 x = (noisetable.Var1(1:10000)-1855)/500;
 d = filt(x) + sin(0:.005:49.995)'; % Noise + Signal
 [y, err] = blms(x, d);
 subplot(2,1,1);
 plot(d);
 title('Noise + Signal');
 subplot(2,1,2);
 plot(err);
 title('Signal');