%% Simulation World
% Engineer: Tim Brothers
% Overview
%    This is a simulation for the Robotic Orchestra
% Design Name:   The Conductor
% File Name:     simulation_world.m
%
% Inputs: 
%		Tempo: controls the tempo of the song. This is a number between 0 and 1
%		Octive: The ocive of the song (normally 4)
%		fs_kHz: sampling rate in kHz.
%       tempo_resolution: this is a value between 0 and 1. 1 being full 
%           resolution. Be careful setting this too high. 1 will lock up
%           your computer.
%       time_offset: This is the delay to the start of the song. This is an
%           index to the time scale, so the actual delay amount is
%           determined by multiple factors
%
% History:       4 January 2020 File created
%
%-----------------------------------------------------

%% Clear the Variables
clc
clear
close all

%% Set up the world
fs_kHz = 5;

% Set the tempo and the octive for the conductor
Octive = 4;
tempo_s = 0.300 %This is the time in seconds for each note

%% Parameters for code simulation
%tempo_resolution = .07; %this is a value between 0 and 1. 1 being full resolution
time_offset = 18;   % This is the delay to the start of the song.
  
%% Create the song given the tempo
[song_freq_Hz, song_duration_s] = conductor_simulation(tempo_s,Octive);

% form up the cumulative durations
cumulative_duration_s = zeros(1, length(song_duration_s));
cumulative_duration_s(1) =  song_duration_s(1);
for i=2:length(song_duration_s)
	cumulative_duration_s(i) = song_duration_s(i) + cumulative_duration_s(i-1);
end

total_duration_s = max(cumulative_duration_s);
fs_Hz = fs_kHz*1000; %Convert kHz to Hz
time_s = 0:(1/fs_Hz):total_duration_s;	%create a time vector

% There is some rounding error happening sometimes, so we are going to make
% the end value exact
time_s(length(time_s)) = total_duration_s;


%% Create the digital signal
min_time_index = 1; %start the min time index at the start
% loop through all the freqs and generate a sin wave for each freq
for i=1:length(song_freq_Hz)
	max_time_index = find(time_s >= cumulative_duration_s(i),1); %find the times that correspond to this duration of time
	digital_note = sin(2*pi*song_freq_Hz(i)*time_s(min_time_index:max_time_index)); %create the actual note
	min_time_index = max_time_index + 1; %shift to the next time region.
	if(i == 1)
        digital = digital_note;
    else
        digital = [digital, digital_note];
    end
    
end

%% Create a signal to test the system
%%Put two copies of the song together
signal = [digital, digital,digital, digital,digital, digital,digital, digital];
signal = circshift(signal, time_offset);

%% Write audio signal
%sound(digital,5000)
audiowrite("rowrow_d18_t300_o4_long.wav",signal,5000);

%% Plot the Time Domain
figure(1)
plot(time_s,digital)
xlabel("time (s)")
ylabel("Amplitude")
title("Row Row Row Your Boat")

%% Plot the frequency
figure(2)
freq_digital = fft(digital);
freq_digital = freq_digital(1:floor(length(digital)/2));
freq_axis_kHz = linspace(0, fs_kHz/2, length(freq_digital));
plot(freq_axis_kHz, abs(freq_digital));
xlabel("Frequency (kHz)");
ylabel("Amplitude");
title("Spectrum of Row Row Row Your Boat");


%% Do the envelope detection
Fs = 5000;
fnyq = Fs/2; % Nyquist Frequncy
y = abs(signal - mean(signal)); %Rectifies signal by removing mean value
time_signal = 0:(1/fs_Hz):total_duration_s*2;

fco = 20; %Final cutoff frequency

% 2nd Butterworth low pass filter
[b,a] = butter(2,fco*1.25/fnyq);

% Zero-phase digital filter
figure(3)
% z = filtfilt(b,a,abs(digital));
% plot(time_s,z,'r');
z = filtfilt(b,a,abs(signal));
%plot(time_signal,z,'r');
%xlabel("Time (s)");
%ylabel("Voltage (V)");

% figure(4)
% plot(time_signal,signal-mean(signal),'b',time_signal,y,'g',time_signal,z,'r');
% xlabel("Time (s)");
% ylabel("Voltage (V)");
% legend('Raw (offset)','Linear envelope');


%% Find the Tempo
% Find difference in time between max and min (i.e. rise and fall of note)
% diff_digital = abs(diff(z));
% diff_digital = diff_digital .* (abs(diff_digital) > .004); % Filter out peaks beneath this level
% x = time_s(1:length(time_s)-1);
% TF = islocalmax(diff_digital);
% %TF2 = islocalmin(diff_digital);
% %Total = TF + TF2;
% %plot(x,diff_digital,x(TF),diff_digital(TF),'r*',x(TF2),diff_digital(TF2),'r*')
% plot(x,diff_digital,x(TF),diff_digital(TF),'r*')
% maxes = x(TF);
% % mins = x(TF2);
% % tempo = abs(mins(1) - maxes(2))
% tempo = abs(maxes(2) - maxes(3))


diff_digital = diff(z);
diff_digital = diff_digital .* (abs(diff_digital) > .004);
x = time_signal(1:length(time_signal)-1);
TF = islocalmax(diff_digital);
TF2 = islocalmin(diff_digital);
Total = TF + TF2;
figure(5)
plot(x,diff_digital,x(TF),diff_digital(TF),'r*',x(TF2),diff_digital(TF2),'r*')
maxes = x(TF);
mins = x(TF2);
tempo = abs(mins(1) - maxes(2))



%% Sync the time
d = finddelay(signal(1:2000),digital(1:2000))
%measured_time_offset_s = z


%measure_time_offset = maxes(1) %This is when we see first peak which is the start of the song
%actual_time_offset_s = time_s(time_offset) %print out the actual start time to compare.

%% Cross correlation
[c,lags] = xcorr(signal,digital);
stem(lags,c);

%% Windowing for N
w = hann(64);
p = w* signal(1:2000);
r = autocorr(p);
Ts = max(r/r(0))

%% Cross correlation through a FFT
% n = 1024;
% x = randn(n,1);
% % cross correlation reference
% xref = xcorr(x,x);
% 
% %FFT method based on zero padding
% fx = fft([x; zeros(n,1)]); % zero pad and FFT
% x2 = ifft(fx.*conj(fx)); % abs()^2 and IFFT
% % circulate to get the peak in the middle and drop one
% % excess zero to get to 2*n-1 samples
% x2 = [x2(n+2:end); x2(1:n)];
% % calculate the error
% d = x2-xref; % difference, this is actually zero
% fprintf('Max error = %6.2f\n',max(abs(d)));
% N =1024
% autocorr = ifft(abs(fft(x, 2*N-1)).^2);
% i = max(abs(autocorr))
% k = find(autocorr==i)
