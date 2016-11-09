function [ sat ] = read_sat(curr_path, hist_path)

    mom = 1; 
    
    % Read the historic tle information (if given)
    if hist_path != -1
        dirs = dir(hist_path);
        mom = length(dirs)-1;
        for ii = 3:length(dirs)
            files = dir(strcat(hist_path,dirs(ii).name,'/'));
            for jj = 3:length(files)
                fid = fopen(strcat(hist_path, dirs(ii).name,'/', files(jj).name));
                fskipl(fid,2);
                kk = 1;
                % Read satellites while storing information as sat(num_sat, paramsm moment)
                while !(isnumeric(t = fgets(fid)))
                    sat(kk*(jj-2),1,ii-2) = str2num(t(3:7));              % Norads ID
                    sat(kk*(jj-2),2,ii-2) = str2num(t(9:16));             % Inclination (degrees)
                    sat(kk*(jj-2),3,ii-2) = str2num(t(18:25));            % Long. asc. node (degrees)
                    sat(kk*(jj-2),4,ii-2) = str2num(t(27:33))/10000000;   % Eccentricity
                    sat(kk*(jj-2),5,ii-2) = str2num(t(35:42));            % Perigee (degrees)
                    sat(kk*(jj-2),6,ii-2) = str2num(t(44:51));            % Mean anomaly (degrees)
                    sat(kk*(jj-2),7,ii-2) = (1/str2num(t(53:63)))*24*60;  % Period (minutes)
                    fskipl(fid,2);
                    kk = kk + 1;
                end
                fclose(fid);
            end
        end
    end
    
    % Read the current tle information
    files = dir(curr_path);
    for ii = 3:length(files)
        fid = fopen(strcat(curr_path,files(ii).name));
        fskipl(fid,2);
        jj = 1;
        % Read satellites while storing information as sat(num_sat, params, moment)
        while !(isnumeric(t = fgets(fid)))
            sat(jj*(ii-2),1,mom) = str2num(t(3:7));              % Norads ID
            sat(jj*(ii-2),2,mom) = str2num(t(9:16));             % Inclination (degrees)
            sat(jj*(ii-2),3,mom) = str2num(t(18:25));            % Long. asc. node (degrees)
            sat(jj*(ii-2),4,mom) = str2num(t(27:33))/10000000;   % Eccentricity
            sat(jj*(ii-2),5,mom) = str2num(t(35:42));            % Perigee (degrees)
            sat(jj*(ii-2),6,mom) = str2num(t(44:51));            % Mean anomaly (degrees)
            sat(jj*(ii-2),7,mom) = (1/str2num(t(53:63)))*24*60;  % Period (minutes)
            fskipl(fid,2);
            jj = jj + 1;
        end
        fclose(fid);
    end  
  
    sat = sat(find(sat(:,1) != 0),:);

end