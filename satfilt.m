function [] = data_filtering(path, num_groups)

    search_path = strcat(path, "output.db");
    csvfile = dir(search_path);   
  
    if numel(csvfile) >= 1
    
      printf("Loading data from %s\n", strcat(path, csvfile.name));
      data = csvread(strcat(path, csvfile.name))(2:end,:);
      
      % acceptable error
      eps_inc = max(data(:,2))/100;
      eps_ecc = max(data(:,3))/100;
      eps_T = max(data(:,4))/100;
      eps_perig = max(data(:,5))/100;
      eps_long_a_n = max(data(:,6))/100;
      eps_m_an = max(data(:,7))/100;
      
      % dynamic ranges
      dr(1) = 0;
      for k = 2:7
        dr(k) = max(data(:,k)) - min(data(:,k));
      end    
    
      for n_param = 2:7  
      
        %find a representative group for inclination
        max_inc = max(data(:,n_param));
        min_inc = min(data(:,n_param));
        delta_inc = (max_inc - min_inc) / num_groups;
        for i = 1:num_groups
          groups_sat{i} = data(find(data(:,n_param) >= (min_inc + delta_inc * (i-1)) ...
                              & data(:,n_param) <= (min_inc + delta_inc * i)),:);
          groups_err{i} = zeros(1, size(groups_sat{i},1));
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
          opt = groups_sat{i}(find(min(groups_err{i}) == groups_err{i}),:)
          if ~(isempty(opt))
            fprintf(out_file, "%10.0f # ", opt(1));
            for j = 2:7
              fprintf(out_file, "%10.6f ", opt(j));
            end
            fprintf(out_file, "\n");
          end
        end  
        
        fclose(out_file);
        
      end

    else
        disp("No output.db file found");
    end


end