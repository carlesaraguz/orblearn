%% Reset environment: ==============================================================================
clear;
close all;
clc;

%% Load extra packages: ============================================================================
pkg load signal;
pkg load geometry;
pkg load parallel;
graphics_toolkit("gnuplot");

path_to_props = "/media/carles/VB2/propagations/historic3_highFs/";
path_to_data  = "/media/carles/VB2/workspace/cds/";
max_files = 6;
load_data = true;  % Set this flag to TRUE to recover previously-generated data from *.mat files.

%% Load propagation files, calculate cross-distances and perform analysis: =========================
if load_data
    matfiles = dir(strcat(path_to_data, "*.mat"));  % Find all data files in this folder.
    items = min(numel(matfiles), nchoosek(max_files, 2));
    printf("[    #] \x1b[33mInfo: loading %d cross-distances from *.MAT files\x1b[0m\n", items);
    if numel(matfiles) > 0
        for kk = 1:items
            printf("[%5d] Loading cross-distance. ", kk);
            fflush(stdout);
            loaded_structre = load(strcat(path_to_data, matfiles(kk).name));
            printf("Copying data for %5u and %5u: ", loaded_structre.p_k(1), loaded_structre.p_k(2));
            fflush(stdout);
            % Copy and rename variables from loaded_structre to cds structs:
            cds(kk).d      = loaded_structre.d_k;
            cds(kk).p      = loaded_structre.p_k;
            cds(kk).tstart = loaded_structre.tstart_k;
            cds(kk).tend   = loaded_structre.tend_k;
            cds(kk).tstep  = loaded_structre.tstep_k;
            printf("\x1b[35m%5d pp\x1b[0m (\x1b[32mdone\x1b[0m)\n", size(loaded_structre.d_k, 1));
            fflush(stdout);
        end
    else
        printf("[   !!] \x1b[31;1mThe folder `%s` does not have *.MAT files\x1b[0m\n", csvfiles(ii).name);
        cds = zeros(0, 0);
        items = 0;
    end
else
    [cds, items] = orblearnLoad3(path_to_props, 1000, max_files, "rand", false, path_to_data);
end

%% Perform frequency analysis: =====================================================================
% -- Parallel execution:
% pararrayfun(4, @orblearnSingleEventAnalysis, cds, [1:items], "UniformOutput", false);
% -- Sequential execution:
% arrayfun(@orblearnSingleEventAnalysis, ctds, [1:items]);
% -- Test execution:
% orblearnSingleEventAnalysis(cdts(1));
% -- Sequential test execution:
for ii = 1:items
    orblearnSingleEventAnalysis(cds(ii), ii);
end
