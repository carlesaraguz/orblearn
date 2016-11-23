%% Reset environment: ==============================================================================
clear;
close all;
clc;

%% Load extra packages: ============================================================================
pkg load signal;
pkg load geometry;
pkg load parallel;
graphics_toolkit("gnuplot");

%% Load propagation files, calculate cross-distances and perform analysis: =========================
[cds, items] = orblearnLoad3("../propagations/historic1/", 1000, 5, "reverse");

%% Perform frequency analysis: =====================================================================
% -- Parallel execution:
pararrayfun(4, @orblearnSingleFreqAnalysis, cds, "UniformOutput", false);
% -- Sequential execution:
% arrayfun(@orblearnSingleFreqAnalysis, cds);
