0%% Cartesian 3D Printer - Digital twin
%
% The following example derived from this Matlab® example:
% https://it.mathworks.com/help/physmod/sm/ug/cartesian_3d_printer.html
%
% This example aims to show a basic digital twin between a real cartesian
% 3D printer and a simulated one. The real printer behaviour is represented 
% by the c-cnc while the simulated one is represented by a Matlab/Simulink
% simulation.
%
%% Project setup
clc;
clear all;

% Add printer data folders
addpath('Printer/Images')
addpath('Printer/CAD')
addpath('s-functions')

%% MQTT client
M = mqttclient('tcp://localhost'); % Broker connection

% Init subscribers
sub = subscribe(M, 'c-cnc/setpoint', Callback=@get_setpoint); % Subscribe to set-point topic
error_sub    = subscribe(M, 'c-cnc/status/error'); % Subscribe to rapid movement topic (error)
position_sub = subscribe(M, 'c-cnc/status/position'); % Subscribe to rapid movement topic (position)

% Publish first set point
write(M, 'c-cnc/setpoint', '{"x":0.000,"y":0.000,"z":-100.000,"rapid":false}'); % Set-point init
% MQTT JSON message description
% Set point message: c-cnc/sp/{x,y,z,rapid}
% Published by the c-cnc.
% 1. c-cnc is the root
% 2. setpoint is the set-point topic
% 3. {x,y,z,rapid} is the message formatted as x, y and z set points and
%    a rapid movement flag (false=no rapid movement, true=rapid movement).

% Simulation pub messages
% Published by the simulation.
% Rapid JSON message: c-cnc/status/error
%                     c-cnc/status/position
% 1. c-cnc/status/error 
% Euclidean distance between setpoint and printer head position.
% 2. c-cnc/status/position
% Actual printer head position

% Publish dummy rapid messages
% publish(M, 'c-cnc/status/error', '0.000'); % Error
% publish(M, 'c-cnc/status/position', '0.000,0.000,0.000'); % Error

%% Set printer parameters
sm_3d_printer_parameters;

%% Motor parameters
w_m = [0, 1000, 2000, 3000]; % Rotational speed [rpm]
t_m = [1, 1, 0.95, 0] * 200; % Torque [Nm]

figure
plot(w_m, t_m)
xlabel("[rpm]")
ylabel("[Nm]")
grid on
title("DC motor torque curve")

%% Open Simulink model and Mechanics explorer

open_system('sm_3d_printer');
set_param('sm_3d_printer','SimMechanicsOpenEditorOnUpdate','on');