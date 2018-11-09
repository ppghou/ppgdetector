%% Sliding DFT

%% Preparation
clc;clear all;close all;

%% Load data
N = input('Pleases enter number of discrete points:');
N_window = input('Pleases enter length of sliding window:');
W = exp(sqrt(-1)*2*pi/N_window);
ts = 1/8;
t = 0:ts:(N-1)*ts;
t1 = 1:1:N;
% Please choose the signal for analysis
x = 5*sin(1*pi*t.*sin((1:1:N))+pi/6);

%% Calculation based on Sliding DFT
y_sliding(1:N_window) = fft(x(1:N_window),N_window);
for n = N_window:N-N_window
    for k = 1:N_window
        y_sliding(k) = (y_sliding(k)+x(n+N_window)-x(n))*W^(k);
    end
end

%% Figure it and high five!
% an = angle(y)/pi*180;
f = -8*round((N_window-1)/2)/N_window:8/N_window:8*fix((N_window-1)/2)/N_window;
figure(1);
subplot(211);
plot(t,x);
title('source');
grid on;

subplot(212);
plot(f,abs(y_sliding)/N_window);
title('|X(f)|');
grid on;
zoom on;

