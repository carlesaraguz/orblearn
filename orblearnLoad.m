function [d, d_idx, s_ids, p_step] = orblearnLoad(path_to_props)
% ORBLEARNLOAD loads all propagation data (*.prop files) from the specified directory and returns
%   the cross-distance vectors.
%
% Usage:  [d, d_idx, s_ids, p_step] = orblearnLoad("path/to/propagations/folder/")
%
%   where         d ->  The cross distances vectors for all pairs of satellites.
%             d_idx ->  A Look-Up-Table that returns the cross-distance row/column correspondance
%                       with any given pair of satellites. E.g. d_idx(2, 5) = 3, d(3, :) is the
%                       cross-distance vector that we were looking for.
%             s_ids ->  A Look-Up-Table to link NORAD ID with row/column of any given satellite.
%            p_step ->  The propagation step (in seconds) for each cross distance vector.
%
% Remarks: p_step is not checked when calculating cross distances (i.e. propagations are not
%   interpolated), therefore imposing that ALL *.prop files have to have the same number of steps
%   and must be calculated with the same delta-t.

    search_path = strcat(path_to_props, "*.prop");
    csvfiles = dir(search_path);            % Find all propagation files in this folder.
    s_ids  = zeros(numel(csvfiles), 1);     % Satellite ID's / index LUT.
    p_step = zeros(numel(csvfiles), 1);     % Propagation time step.
    if numel(csvfiles) > 1
        for ii = 1:numel(csvfiles)
            printf("Loading data from %s\n", strcat(path_to_props, csvfiles(ii).name));
            s(:,:,ii) = csvread(strcat(path_to_props, csvfiles(ii).name))(7:end, 5:7);  % Load data.
            s_ids(ii) = sscanf((csvfiles(ii).name), "%d");                              % Save ID.
            p_step(ii) = csvread(strcat(path_to_props, csvfiles(ii).name))(4, 2);       % Save step.
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
end
