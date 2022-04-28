% Analyze executiuon time of timed* examples
clc; clear all; close all;

%% Read files

timed0_csv = readtable('timed1_0.csv');
timed2_csv = readtable('timed2.csv');
timed3_csv = readtable('timed3.csv');

%% Plot
clc; close all;

figure
subplot(3,1,1)
plot(timed0_csv.n, timed0_csv.dt*1000)
xlabel("samples [#]")
ylabel("time [ms]")
title("Timed0 execution time")
grid on
subplot(3,1,2)
plot(timed2_csv.n, timed2_csv.dt*1000)
xlabel("samples [#]")
ylabel("time [ms]")
title("Timed0 execution time")
grid on
subplot(3,1,3)
plot(timed3_csv.n, timed3_csv.dt*1000)
xlabel("samples [#]")
ylabel("time [ms]")
title("Timed0 execution time")
grid on

%% Distribution
clc; close all;

figure
subplot(3,1,1)
histogram(timed0_csv.dt*1000, 500)
title("Execution time histogram")
ylabel("frequency")
xlabel("value [ms]")
xlim([0 10])
grid on
subplot(3,1,2)
histogram(timed2_csv.dt*1000, 500)
title("Execution time histogram")
ylabel("frequency")
xlabel("value [ms]")
xlim([0 10])
grid on
subplot(3,1,3)
histogram(timed3_csv.dt*1000, 500)
title("Execution time histogram")
ylabel("frequency")
xlabel("value [ms]")
xlim([0 10])
grid on

%% Get some statistics

print_exe_time_stats(timed0_csv.dt, 'timed0');
print_exe_time_stats(timed2_csv.dt, 'timed2');
print_exe_time_stats(timed3_csv.dt, 'timed3');

