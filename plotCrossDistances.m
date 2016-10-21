function [ ] = plotCrossDistances(d, d_idx, p_step, s_ids, s_ids1, s_ids2, time_start_days, time_end_days)
%PLOTS the cross distances between the satellites with NORAD's id s_ids1 and s_ids2 in time_start and time_end
  
  idx1 = find(s_ids == str2num(s_ids1));
  idx2 = find(s_ids == str2num(s_ids2));
  
  cross_distance = d(d_idx(idx1, idx2),:);
  time_step_sec =  p_step(d_idx(idx1, idx2));
 
  time_max_days = length(cross_distance) * time_step_sec / (60 *60 *24);
  if time_end_days > time_max_days
    disp(["Desired end time (" num2str(time_end_days) " days) out of limits."])
    disp(["It will be plot until " num2str(time_max_days)  " days."])
    clear time_end_days
    time_end_days = time_max_days;
  end
  
  time_start_steps = time_start_days * (24 * 60 * 60) / time_step_sec;
  time_end_steps = time_end_days * (24 * 60 * 60) / time_step_sec;
  time_steps = time_start_steps:time_end_steps;
  time_days = linspace(time_start_days, time_end_days, length(time_steps));
  
  cross_distance_show = cross_distance(time_steps);
  
  h = figure();
  plot(time_days, cross_distance_show, 'r-')
  title(["Cross distance between " s_ids1 " and " s_ids2])
  xlabel("Time (days)")
  ylabel("Cross distance (kms)")

  % save(h, plot_name.png)
  
end