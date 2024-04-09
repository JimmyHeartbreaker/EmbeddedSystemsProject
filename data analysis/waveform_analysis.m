clear();
x = readtable("waveform.csv");
plot(x.Var1)
%axis([0 100 0 4000])
n = length(x.Var1);       % original sample length
%n = pow2(nextpow2(m));  % transform length
y = fft(x.Var1); %,n);
y = fftshift(y);% DFT of signal
fs = 32000;
f = (0:n-1)*(fs/n)/10;
power = abs(y).^2/n;      

plot(f, power); %(1:floor(n/2)),power(1:floor(n/2)))
xlabel('Frequency')
ylabel('Power')