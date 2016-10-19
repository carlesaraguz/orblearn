%% Reset environment -------------------------------------------------------------------------------
clear;
close all;
clc;

%% Load propagation files --------------------------------------------------------------------------
csvfiles = dir("propagations/*.prop");      % Find all propagation files in this folder.
s_ids = zeros(numel(csvfiles), 1);          % Satellite ID's / index LUT.
if numel(csvfiles) > 1
    for ii = 1:numel(csvfiles)
        printf("Loading data from %s\n", strcat("propagations/",csvfiles(ii).name));
        s(:,:,ii) = csvread(strcat("propagations/",csvfiles(ii).name))(2:end,5:7);      % Load data.
        s_ids(ii) = sscanf((csvfiles(ii).name), "%d");                                  % Save ID.
    end

    n_sats = size(s,3);                                     % Number of satellites.
    relations = (n_sats ^ 2) - ((1 + n_sats) * n_sats / 2); % Number of sat-sat distances.
    d = zeros(relations, size(s,1));                        % Distances matrix.
    d_idx = zeros(n_sats);                                  % Distances index LUT.
    printf("Calculating distances between %d satellites (%d)\n", n_sats, relations);
    rel = 1;
    for ii = 1:size(s,3)
        for jj = ii+1:size(s,3)
            d(rel,:) = sqrt((s(:,1,ii) - s(:,1,jj)).^2 + (s(:,2,ii) - s(:,2,jj)).^2 + (s(:,3,ii) - s(:,3,jj)).^2)';
            d_idx(ii,jj) = rel; d_idx(jj,ii) = rel;
            rel++;
        end
    end
else
    disp("Less than 2 propagation files found");
end
