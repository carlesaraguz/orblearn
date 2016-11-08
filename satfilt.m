t%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This function filters the data of the satellites presented in the output.db
% file expecting it to be a CSV file. The criterion to do it is the following:
% path - Path where it will find the output.db file and where it will store the
%   resulting .db file.
% n_param - Parameter that is expected to filter:
%   2  - inclination (degrees)
%   3  - eccentricity
%   4  - period (minutes)
%   5  - perigee (degrees)
%   6  - longitude of the ascending node (degrees)
%   7  - mean anomaly (degrees)
%   -1 - all of them 
% max_val and min_val are the maximum and minimum values for the parameter 
%   selected. (If all the parameteres are being filtered or they are out of 
%   range, they will be computed automatically)
% num_groups - Number of satellites that you desire to achieve after the 
%   filtering (it may be less). If its value is different from -1, the group 
%   list will be ignored. 
% groups_lims - Array with the limits a the group as an array.
%   E.g.: [[15,30];[45,65];...;[329, 354]] 
%   The program will not controle if they are disjount groups or not.
%   If there is a number of groups specified, this option will be ignored.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [] = satfilt(path, n_param, min_val, max_val, num_groups, groups_lims)

    search_path = strcat(path, "output.db");
    csvfile = dir(search_path);
    
    if numel(csvfile) >= 1
   
        printf("Loading data from %s\n", strcat(path, csvfile.name));
        data = csvread(strcat(path, csvfile.name))(2:end,:);
        
        % dynamic range
        dr(1) = 0;
        for k = 2:7
            dr(k) = max(data(:,k)) - min(data(:,k));
        end
        
        if n_param == -1
            n_min = 2;
            n_max = 7;
            min_val = 0;
            max_val = 1e6; % Infinitum
        else 
            n_min = n_param;
            n_max = n_param;
        end
        
        for n_param = n_min:n_max
        
            % compute max and min values
            max_val = min(max(data(:,n_param)), max_val);
            min_val = max(min(data(:,n_param)), min_val);
            
            % find a representative group for the parameter
            if num_groups == -1
                for i = 1:length(groups_lims)
                    num_groups = size(groups_lims,2);
                    groups_sat{i} = data(find(data(:,n_param) >= groups_lims(i,1) ...
                                           & data(:,n_param) <= groups_lims(i,2)),:);
                    groups_err{i} = zeros(1, size(groups_sat{i},1));
                end
            else
                delta_val = (max_val - min_val) / num_groups;
                for i = 1:num_groups
                    groups_sat{i} = data(find(data(:,n_param) >= (min_val + delta_val * (i-1)) ...
                                           & data(:,n_param) <= (min_val + delta_val * i)),:);
                    groups_err{i} = zeros(1, size(groups_sat{i},1));
                end 
            end

            % find the error of each value in each group
            for i = 1:num_groups
                for j = 1:num_groups
                    if i ~= j
                        for ii = 1:(size(groups_sat{i},1)-1)
                            for jj = 1:(size(groups_sat{j},1)-1)
                                aux = 0;
                                for k = 2:7
                                    if k ~= n_param
                                    aux = aux + abs(groups_sat{i}(ii,k) - ...
                                                   groups_sat{j}(jj,k)) / dr(k);
                                    end
                                end
                                groups_err{j}(jj) = groups_err{j}(jj) + aux;
                            end
                        end
                    end
                end  
            end
            
            % create a destination file
            if n_param == 2
                name = "inclination";
            elseif n_param == 3
                name = "eccentricity";
            elseif n_param == 4
                name = "period";
            elseif n_param == 5
                name = "perigee";
            elseif n_param == 6
                name = "longitude-ascending-node";
            else
                name = "mean-anomaly";
            end 
            out_path = strcat(path, name, ".db");
            out_file=fopen(out_path,'w');
            
            % write the header of the file
            fprintf(out_file, "NORAD's ID # inc ecc period perig long_a_n m_an \n");
                
            % find the best value of each group and write it in the file
            for i = 1:num_groups
                opt = groups_sat{i}(find(min(groups_err{i}) == groups_err{i}),:);
                if ~(isempty(opt))
                    fprintf(out_file, "%10.0f # ", opt(1));
                    for j = 2:7
                        fprintf(out_file, "%10.6f ", opt(j));
                    end
                    fprintf(out_file, "\n");
                end
            end  
            
            fclose(out_file);
            
            % reset values
            min_val = 0;
            max_val = 1e6; % Infinitum
            
        end
    
    else
        disp("No output.db file found")
    end

end
