%% Simple axis example
% Model parameters
% Base geometry
lb = 500;
wb = 25;
tb = 15;
% Cursor geometry
t = tb;
w = wb;
l = w;
% Lead screw head
nls = 6;
rls = 20;
lls = 50;

% Motor parameters
w_m = [0, 1000, 2000, 3000]; % Rotational speed [rpm]
t_m = [1, 1, 0.95, 0] * 200; % Torque [Nm]

figure
plot(w_m, t_m)
xlabel("[rpm]")
ylabel("[Nm]")
grid on
title("DC motor torque curve")

%% Open Simulink model
open_system('simple_axis');