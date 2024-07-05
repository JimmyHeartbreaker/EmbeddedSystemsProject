function y = doFilter(x)
%DOFILTER Filters input x and returns output y.

% MATLAB Code
% Generated by MATLAB(R) 23.2 and DSP System Toolbox 23.2.
% Generated on: 26-Apr-2024 19:51:20

%#codegen

% To generate C/C++ code from this function use the codegen command.
% Type 'help codegen' for more information.

persistent Hd;

if isempty(Hd)
    
    % The following code was used to design the filter coefficients:
    %
    % N    = 10;     % Order
    % F3dB = 1000;   % 3-dB Frequency
    % Fs   = 32000;  % Sampling Frequency
    %
    % h = fdesign.lowpass('n,f3db', N, F3dB, Fs);
    %
    % Hd = design(h, 'butter', ...
    %     'SystemObject', true,...
    %      UseLegacyBiquadFilter=true);
    
    Hd = dsp.BiquadFilter( ...
        'Structure', 'Direct form II', ...
        'SOSMatrix', [1 2 1 1 -1.90347858314633 0.940769933214891; 1 2 1 1 ...
        -1.8019714740082 0.837274182242373; 1 2 1 1 -1.72377617276251 ...
        0.757546944478829; 1 2 1 1 -1.67109035577047 0.70382895131076; 1 2 1 1 ...
        -1.64466301584778 0.676883869190622], ...
        'ScaleValues', [0.00932283751714132; 0.00882567705854282; ...
        0.00844269292907995; 0.00818464888507354; 0.00805521333571095; 1]);
end

s = double(x);
y = step(Hd,s);
