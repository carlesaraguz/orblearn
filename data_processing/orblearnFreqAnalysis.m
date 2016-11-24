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
[cds, items] = orblearnLoad3("/media/carles/VB2/propagations/historic1/", 1000, 90, "rand");

%% Perform frequency analysis: =====================================================================
% -- Parallel execution:
pararrayfun(4, @orblearnSingleFreqAnalysis, cds, [1:items], "UniformOutput", false);
% -- Sequential execution:
% arrayfun(@orblearnSingleFreqAnalysis, cds, [1:items]);
% -- Test execution:
% orblearnSingleFreqAnalysis(cds(1));
