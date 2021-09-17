Fs = 22050;
numSamples = 10000;
DownsampleFactor = 15;
frameSize = 10*DownsampleFactor;

sine = dsp.SineWave([0.4 1],[10 200], ...
    'SamplesPerFrame',frameSize, ...
    'SampleRate',Fs);

% Low pass filter
lp1 = dsp.FIRFilter('Numerator',firpm(20,[0 0.03 0.1 1],[1 1 0 0]));

% Hilbert Transform
N = 60; % Filter order
hilbertTransformer = dsp.FIRFilter( ...
        'Numerator',firpm(N,[0.01 .95],[1 1],'hilbert'));
delay = dsp.Delay('Length',N/2);
lp2 = dsp.FIRFilter('Numerator',firpm(20,[0 0.03 0.1 1],[1 1 0 0]));

scope1 = timescope( ...
  'Name','Envelope detection using Amplitude Modulation', ...
  'SampleRate',[Fs,Fs/DownsampleFactor], ...
  'TimeDisplayOffset',[(N/2+frameSize)/Fs,0], ...
  'TimeSpanSource','property', ...
  'TimeSpan',0.45, ...
  'YLimits',[-2.5 2.5], ...
  'Position', [100 200 560 500]);
pos = scope1.Position;

scope2 = timescope( ...
  'Name','Envelope detection using Hilbert Transform', ...
  'Position',[pos(1) + pos(3), pos(2:4)], ...
  'SampleRate',[Fs,Fs/DownsampleFactor], ...
  'TimeDisplayOffset',[(N/2+frameSize)/Fs,0], ...
  'TimeSpanSource','Property', ...
  'TimeSpan',0.45, ...
  'YLimits',[-2.5 2.5]);


for i = 1:numSamples/frameSize
    sig = sine();
    sig = (1 + sig(:,1)) .* sig(:, 2);      % Amplitude modulation

    % Envelope detector by squaring the signal and lowpass filtering
    sigsq = 2 * sig .* sig;
    sigenv1 = sqrt(lp1(downsample(sigsq,DownsampleFactor)));

    % Envelope detector using the Hilbert transform in the time domain
    sige = abs(complex(0, hilbertTransformer(sig)) + delay(sig));
    sigenv2 = lp2(downsample(sige,DownsampleFactor));

    % Plot the signals and envelopes
    scope1(sig,sigenv1);
    scope2(sig,sigenv2);
end
release(scope1);
release(scope2);













