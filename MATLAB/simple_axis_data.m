%% Model parameters
% base geometry
lb = 500;
wb = 25;
tb = 15;
% cursor geometry
t = tb;
w = wb;
l = w;
% Lead screw head
nls = 6;
rls = 20;
lls = 50;

%% Motor
w_m = [-1500, -1000, -500, 0, 500, 1000, 1500, 2000, 2500, 3000];
t_m = [.04, .035, .03, .025, .02, .015, .01, .005, 0, -.005] * 200;
w_m = [0, 1000, 2000, 3000];
t_m = [1, 1, 0.95, 0] * 200;
plot(w_m, t_m)

%% Actuation
s_x = [100 100 0 0 -100 -100].';
s_t = [0 2 4 6 7 10].';
plot(s_t, s_x)

%% MQTT
M = mqtt('tcp://localhost');
sub = subscribe(M, '/sp');
