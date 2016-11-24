function orblearnSingleDerivative (cds_i, job_id = 0, rzs = true)
% ORBLEARNSINGLEDERIVATIVE help TBD

    % Recover signal details: ----------------------------------------------------------------------
    Ts = cds_i.tstep;                     % Known sampling period (i.e. propagation step).
    Fs = 1 / Ts;                            % Known sampling frequency.

    % Reconstruct signal (linearly interpolating points): ------------------------------------------
    t = [cds_i.tstart:Ts:cds_i.tend];   % Recovered time.
    x0 = cds_i.d(:, 2);                   % Original distance.
    t0 = cds_i.d(:, 1);                   % Original time.
    x = interp1(t0, x0, t, "linear");       % Recovered distance.
    t = (t - min(t)) / 3600;                % Start in 0 and display in hours.

    % Calculate derivatives: -----------------------------------------------------------------------
    dx  = ([  x(2:end)   x(end)] - x) / Ts;
    d2x = ([ dx(2:end)  dx(end)] - dx) / Ts;
    d3x = ([d2x(2:end) d2x(end)] - d2x) / Ts;
    d4x = ([d3x(2:end) d3x(end)] - d3x) / Ts;

    % Remove 0-samples (optional): -----------------------------------------------------------------
    rzs = true;                     % TRUE = remove 0-samples.
    if rzs
        t1 = t(find( dx != 0));
        t2 = t(find(d2x != 0));
        t3 = t(find(d3x != 0));
        t4 = t(find(d4x != 0));
        dx  =  dx(find( dx != 0));
        d2x = d2x(find(d2x != 0));
        d3x = d3x(find(d3x != 0));
        d4x = d4x(find(d4x != 0));
        note_str = "\n(* zero-valued samples removed!)";
    else
        t1 = t;
        t2 = t;
        t3 = t;
        t4 = t;
        note_str = " ";
    end

    % Calculate spectrum (PSD): --------------------------------------------------------------------
    Nfft = 2 ** (nextpow2(length(t)) - 2);  % Number of FFT samples.
    f = (Fs * (0:(Nfft/2 - 1)) / Nfft);     % Frequency axis (in Hz).
    [p0, ff0] = periodogram(  x,   ones(size(x)), Nfft, Fs, "onesided");  % Generate PSD of x.
    [p1, ff1] = periodogram( dx,  ones(size(dx)), Nfft, Fs, "onesided");  % Generate PSD of dx.
    [p2, ff2] = periodogram(d2x, ones(size(d2x)), Nfft, Fs, "onesided");  % Generate PSD of d2x.
    [p3, ff3] = periodogram(d3x, ones(size(d3x)), Nfft, Fs, "onesided");  % Generate PSD of d3x.
    [p4, ff4] = periodogram(d4x, ones(size(d4x)), Nfft, Fs, "onesided");  % Generate PSD of d4x.

    % Estimate probability density function (PDF) with KDE: ----------------------------------------
    kxi   = min(  x); kxf   = max(  x);
    kdxi  = min( dx); kdxf  = max( dx);
    kd2xi = min(d2x); kd2xf = max(d2x);
    kd3xi = min(d3x); kd3xf = max(d3x);
    kd4xi = min(d4x); kd4xf = max(d4x);

    kx   = [kxi:(kxf-kxi)/300:kxf];
    kdx  = [kdxi:(kdxf-kdxi)/300:kdxf];
    kd2x = [kd2xi:(kd2xf-kd2xi)/300:kd2xf];
    kd3x = [kd3xi:(kd3xf-kd3xi)/300:kd3xf];
    kd4x = [kd4xi:(kd4xf-kd4xi)/300:kd4xf];

    kde_x   = kernel_density(  kx',   x', 10);
    kde_dx  = kernel_density( kdx',  dx',  0.3);
    kde_d2x = kernel_density(kd2x', d2x',  0.01);
    kde_d3x = kernel_density(kd3x', d3x',  0.00009);
    kde_d4x = kernel_density(kd4x', d4x',  0.000005);

    % Plot results: --------------------------------------------------------------------------------
    color_blue0 = [ 19  95 216] / 255;
    color_blue1 = [ 74 147 232] / 255;
    color_blue2 = [109 160 219] / 255;
    color_blue3 = [132 165 204] / 255;
    color_blue4 = [133 150 170] / 255;

    fig = figure(1, "visible", "off");

    subplot(5, 5, [1 2]);
    plot(t(1:(end/6)),   x(1:(end/6)), "color", color_blue0, "linewidth", 0.5);
    title(strcat(["Cross-distance (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"]));
    xlabel("Time (hours)");
    ylabel("Cross-distance (km)");
    axis tight;
    subplot(5, 5, [3]);
    semilogx(ff0(2:end), 20 * log10(p0(2:end)), "color", color_blue0, "linewidth", 0.5);
    title("Power Spectral Density (PSD)");
    xlabel("Frequency (Hz)");
    ylabel("PSD (dB)");
    grid on;
    axis tight;
    subplot(5, 5, [4 5]);
    plot(kx, kde_x, "color", color_blue0, "linewidth", 2);
    xlim([850 1000]);
    title("Estimated PDF (KDE)");


    subplot(5, 5, [6 7]);
    stem(t1, dx, "color", color_blue1, "markeredgecolor", color_blue1, "linewidth", 0.5);
    title(strcat(["1^{st} derivative cross-distance (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"]));
    xlabel("Time (hours)");
    ylabel("d/dt (km/s)");
    axis([t(1) t(end/6) (min(dx) * 1.1) (max(dx) * 1.1)]);
    subplot(5, 5, [8]);
    semilogx(ff1(2:end), 20 * log10(p1(2:end)), "color", color_blue1, "linewidth", 0.5);
    title("Power Spectral Density (PSD)");
    xlabel("Frequency (Hz)");
    ylabel("PSD (dB)");
    grid on;
    axis tight;
    subplot(5, 5, [9 10]);
    plot(kdx, kde_dx, "color", color_blue1, "linewidth", 2);
    title(strcat(["Estimated PDF (KDE)" note_str]));
    axis tight;


    subplot(5, 5, [11 12]);
    stem(t2, d2x, "color", color_blue2, "markeredgecolor", color_blue2, "linewidth", 0.5);
    title(strcat(["2^{nd} derivative cross-distance (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"]));
    xlabel("Time (hours)");
    ylabel("d^2/dt (km/s^2)");
    axis([t(1) t(end/6) (min(d2x) * 1.1) (max(d2x) * 1.1)]);
    subplot(5, 5, [13]);
    semilogx(ff2(2:end), 20 * log10(p2(2:end)), "color", color_blue2, "linewidth", 0.5);
    title("Power Spectral Density (PSD)");
    xlabel("Frequency (Hz)");
    ylabel("PSD (dB)");
    grid on;
    axis tight;
    subplot(5, 5, [14 15]);
    plot(kd2x, kde_d2x, "color", color_blue2, "linewidth", 2);
    title(strcat(["Estimated PDF (KDE)" note_str]));
    axis tight;


    subplot(5, 5, [16 17]);
    stem(t3, d3x, "color", color_blue3, "markeredgecolor", color_blue3, "linewidth", 0.5);
    title(strcat(["3^{rd} derivative cross-distance (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"]));
    xlabel("Time (hours)");
    ylabel("d^3/dt (km/s^3)");
    axis([t(1) t(end/6) (min(d3x) * 1.1) (max(d3x) * 1.1)]);
    subplot(5, 5, [18]);
    semilogx(ff3(2:end), 20 * log10(p3(2:end)), "color", color_blue3, "linewidth", 0.5);
    title("Power Spectral Density (PSD)");
    xlabel("Frequency (Hz)");
    ylabel("PSD (dB)");
    grid on;
    subplot(5, 5, [19 20]);
    plot(kd3x, kde_d3x, "color", color_blue3, "linewidth", 2);
    title(strcat(["Estimated PDF (KDE)" note_str]));
    axis tight;


    subplot(5, 5, [21 22]);
    stem(t4, d4x, "color", color_blue4, "markeredgecolor", color_blue4, "linewidth", 0.5);
    title(strcat(["4^{th} derivative cross-distance (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"]));
    xlabel("Time (hours)");
    ylabel("d^4/dt (km/s^4)");
    axis([t(1) t(end/6) (min(d4x) * 1.1) (max(d4x) * 1.1)]);
    subplot(5, 5, [23]);
    semilogx(ff4(2:end), 20 * log10(p4(2:end)), "color", color_blue4, "linewidth", 0.5);
    title("Power Spectral Density (PSD)");
    xlabel("Frequency (Hz)");
    ylabel("PSD (dB)");
    grid on;
    subplot(5, 5, [24 25]);
    plot(kd4x, kde_d4x, "color", color_blue4, "linewidth", 2);
    title(strcat(["Estimated PDF (KDE)" note_str]));
    axis tight;

    h = findall(0, "-property", "fontname");
    set(h, "fontname", "CMU Sans Serif");
    h = findall(0, "-property", "markersize");
    set(h, "markersize", 2);

    % Save in file: --------------------------------------------------------------------------------
    filename = strcat(["/media/carles/VB2/plots/plots2/fig_" int2str(job_id) "_" int2str(cds_i.p(1)) "-" int2str(cds_i.p(2)) ".svg"]);
    print(fig, "-dsvg", filename, "-S1700,1000");
    printf(" -- Saving plot: \x1b[32m%s\x1b[0m\n", filename);
    fflush(stdout);

end
