function [] = deltaStatistics(path_to_single, path_to_corr)  
% % DELTASTATISTICS  takes both the propagation of different satellites 
% by simple propagation of its TLE and the corrected propagation taking 
% two different TLE a day

    % Taking the 6 orbital parameter information (?)

    % Taking the simple TLE propagation of the satellites
    single_path = strcat(path_to_single, "*.prop");
    csvfiles = dir(single_path);            % Find all propagation files in this folder.
    norads  = zeros(numel(csvfiles), 1);     % Satellite ID's / index LUT.
    p_step = zeros(numel(csvfiles), 1);     % Propagation time step.
    for ii = 1:numel(csvfiles)
        printf("Loading data from %s\n", strcat(path_to_single, csvfiles(ii).name));
        s_p(:,:,ii) = csvread(strcat(path_to_props, csvfiles(ii).name))(7:end,5:7);   % Load data.
        s_ids_p(ii) = sscanf((csvfiles(ii).name), "%d");                              % Save ID.
        p_step(ii) = csvread(strcat(path_to_single, csvfiles(ii).name))(4,2);        % Save step.
    end
    
    % Taking the  corrected TLE propagation of the satellites
    corr_path = strcat(path_to_corr, "*/");
    for ii = 1:numel(corr_path)
        corr_path_files = strcat(corr_path, "*.prop");
        csvfiles = dir(corr_path_files);
        s_ids_c = zeros(numel(csvfiles),1);
        for jj = 1:numel(csvfiles)
            printf("Loading data from %s\n", strcat(corr_path, csvfiles(jj).name));
            s_c(:,:,jj) = csvread(strcat(corr_path, csvfiles(jj).name))(7:end,5:7);
        end
    end
    
    % Calculating the se of the propagated and corrected satellites
    n_sats = size(s,3);
    for ii = 1:n_sats
        se(ii,:) = sqrt((s_p(:,1,ii) - s_c(:,1,ii)).^2 + (s_p(:,2,ii) - s_c(:,2,ii)).^2 + (s_p(:,3,ii) - s_c(:,3,ii)).^2)';
    end
    
    % Calculating the mean and variance of the se
    m_se = mean(se);
    v_se = var(se);
    
    % Storing this information
    for ii = 1:n_sats
        h_se = figure();
        plot(se(ii,:), 'r-')
        title(["Cross distance between " num2str(s_ids1) " and " num2str(s_ids2)])
        xlabel("Time (one week)")
        ylabel("Squared error")
        legend(["MSE: ", num2str(m_se(ii)), "\nVar(SE): ", num2str(v_se(ii))])
        grid on
        save(h_se, strcat("./plots/",num2str(norads(ii)),".png"))        
    end 

end 