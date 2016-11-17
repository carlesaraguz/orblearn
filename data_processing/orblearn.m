%% Reset environment -------------------------------------------------------------------------------
clear;
close all;
clc;

%% Load propagation files --------------------------------------------------------------------------
[d, d_idx, s_ids, p_step] = orblearnLoad("propagations/");
