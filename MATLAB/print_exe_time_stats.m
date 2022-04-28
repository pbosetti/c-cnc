function print_exe_time_stats(delta_time,name)
%PRINT_EXE_TIME_STATS Summary of this function goes here
%   Detailed explanation goes here
    mean_time = mean(delta_time);
    std_dev_time = std(delta_time);

    disp(name);
    disp(['mean value: ', num2str(mean_time*1000), ' +/- ', num2str(std_dev_time*1000), ' ms'])
end

