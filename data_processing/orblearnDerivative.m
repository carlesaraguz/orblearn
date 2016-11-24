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
[cds, items] = orblearnLoad3("/media/carles/VB2/propagations/historic1/", 1000, 40, "rand");

%% Perform analysis of the derivative: =============================================================
% -- Parallel execution:
pararrayfun(4, @orblearnSingleDerivative, cds(randperm(items)), [1:items], "UniformOutput", false);
% -- Sequential execution:
% arrayfun(@orblearnSingleDerivative, cds, [1:items]);
% -- Test execution:
% orblearnSingleDerivative(cds(1));
% -- Sequential test execution:
% for ii = 1:items
%     orblearnSingleDerivative(cds(ii), ii);
% end
