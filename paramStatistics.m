function[] = paramStatistics(curr_path, hist_path)

    % We get the satellites information
    sat = read_sat(curr_path, hist_path);
    
    % Now we take the statistics of every parameter for every satellites
    err = diff(sat(:,2:end,:),1,3);    % Difference of the parameters through time
    m_err = mean(err,3);
    v_err = var(err,1,3);
    max_err = max(err,1,3);
    min_err = min(err,1,3); 
    
    m_m_err = mean(m_err);
    m_v_err = mean(v_err);
    m_max_err = mean(max_err);
    m_min_err = mean(min_err);  
    
    % Create a directory for saving the plots
    mkdir('./plots/')
    mkdir('./plots/paramStatistics/')
    mkdir(strcat('./plots/paramStatistics/',datestr(now)(1:11),'/'));
    output_dir = strcat('./plots/paramStatistics/',datestr(now)(1:11),'/',...
                        datestr(now)(13:20),'/');
    mkdir(output_dir);
                  
    % Plot and save plots
    for ii = 1:6
        if ii == 1
            name = 'inclination';
            precision = 'degrees';
        elseif ii == 2
            name = 'long_a_n';
            precision = 'degrees';
        elseif ii == 3
            name = 'eccentricity';
            precision = 'no value';
        elseif ii == 4
            name = 'perigee';
            precision = 'degrees';
        elseif ii == 5
            name = 'm_anomaly';
            precision = 'degrees';
        else
            name = 'period';
            precision = 'minutes';
        end 
        
        n = 1:size(sat,1);
        
        h = figure(ii);
        subplot(2,2,1)
        plot(n,m_err(:,ii),'r*')
        hold on
        plot(n,m_m_err(ii)*ones(size(m_err,1),1),'b-')
        title(strcat('Mean (',num2str(m_m_err(ii)),')'))
        ylabel(precision)
        xlabel('sats')       
        subplot(2,2,2)
        plot(n,v_err(:,ii),'r*')
        hold on
        plot(n,m_v_err(ii)*ones(size(v_err,1),1),'b-')
        title(strcat('Variance (',num2str(m_v_err(ii)),')'))
        ylabel(precision)
        xlabel('sats')
        subplot(2,2,3)
        plot(n,max_err(:,ii),'r*')
        hold on
        plot(n,m_max_err(ii)*ones(size(max_err,1),1),'b-')
        title(strcat('Maximums (',num2str(m_max_err(ii)),')'))
        ylabel(precision)
        xlabel('sats')
        subplot(2,2,4)
        plot(n,min_err(:,ii),'r*')
        hold on
        plot(n,m_min_err(ii)*ones(size(min_err,1),1),'b-')
        title(strcat('Minimums (',num2str(m_min_err(ii)),')'))
        ylabel(precision)
        xlabel('sats')
        print(h,strcat(output_dir, name,'.jpg'));
    
    end
end
    
          