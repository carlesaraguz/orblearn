function [P, info] = orblearnLoad2(path_to_props)
% ORBLEARNLOAD2 loads all propagation data (*.prop files) from the specified directory and returns
%   unprocessed data.
%
% Usage:  [P, info] = orblearnLoad("path/to/propagations/folder/")
%
%   where         P ->  The propagation matrix for each satellite.
%              info ->  Information extracted from the headers, for each satellite.
%

    search_path = strcat(path_to_props, "*.prop");
    csvfiles = dir(search_path);            % Find all propagation files in this folder.
    info = zeros(4);                        % Information matrix.
    if numel(csvfiles) > 0
        for ii = 1:numel(csvfiles)
            printf("Loading data from %s\n", strcat(path_to_props, csvfiles(ii).name));
            P(:,:,ii) = csvread(strcat(path_to_props, csvfiles(ii).name))(7:end, 2:7);  % Load data.
            info      = csvread(strcat(path_to_props, csvfiles(ii).name))(2:5, 2);      % Save info.
        end
    end
end
