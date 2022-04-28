% Single axis example
clc; clear all; close all;

%% Parameters
% Base
lb = 500; % mm
wb = 25; 
tb = 15;

% Cursor
t = tb;
w = wb;
l = w;

%% Motor torque curve

w_m = [0, 1000, 2000, 3000]; % [rpm]
t_m = [1, 1, 0.95, 0] * 2; % [Nm]

figure
plot(w_m, t_m)
grid on
title("Motor torque curve")
xlabel("speed [rpm]")
ylabel("torque [Nm]")

