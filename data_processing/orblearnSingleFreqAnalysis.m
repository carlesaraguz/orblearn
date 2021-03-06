function orblearnSingleFreqAnalysis (cds_i, job_id = 0, force_lp = true)
% ORBLEARNSINGLEFREQANALYSIS help TBD

    Ts = cds_i.tstep;     % Known sampling period (i.e. propagation step).
    Fs = 1 / Ts;          % Known sampling frequency.

    % Reconstruct signal (linearly interpolating points): ------------------------------------------
    t = [cds_i.tstart:Ts:cds_i.tend];       % Recovered time.
    x0 = cds_i.d(:, 2);                     % Original distance.
    t0 = cds_i.d(:, 1);                     % Original time.
    x = interp1(t0, x0, t, "linear");       % Recovered distance.
    t = (t - min(t)) / 3600;                % Start in 0 and display in hours.

    % Calculate FFT samples:
    Nfft = 2 ** 15;                         % Number of FFT samples.
    f = (Fs * (0:(Nfft/2 - 1)) / Nfft);     % Frequency axis (in Hz).

    % Remove f(0) component:
    Af0 = sum(x) / length(x);
    x = x - Af0;

    % Get FFT and PSD: -----------------------------------------------------------------------------
    y = fftshift(abs(fft(x, Nfft)));                                % FFT.
    y = y((Nfft / 2 + 1):end);                                      % Get rid of the imaginary edge.
    [p, ff] = periodogram(x, ones(size(x)), Nfft, Fs, "onesided");  % PSD.

    % Filter FFT: ----------------------------------------------------------------------------------
    filt_n = 4;                             % Filter order (BP4 or LP4).
    filt_attenuation = 40;                  % Min. 40 dB of attenuation in stop-band.

    if force_lp
        % LP-only: ---------------------------------------------------------------------------------
        % -- Processes the specrum to find the true firt harmonic:
        % -- We expect the 1st harmonic to be compressed within f = [4e-4 and 1e-3].
        y2 = y(floor((4e-5 / Fs) * Nfft):ceil((1e-3 / Fs) * Nfft));
        win_size = round(length(y2) / 7);
        maxs = zeros(2, 0);
        for jj = 1:(length(y2) - win_size)
            max_jj   = find(y2 == max(y2(jj:(jj + win_size))));
            maxs_idx = find(max_jj == maxs(1, :));
            if maxs_idx                     % This maximum had been found before.
                maxs(2, maxs_idx)++;        % Increment the counter.
            else                            % This maximum had never been found before.
                maxs = [maxs [max_jj; 1]];  % Save the maximum.
            end
        end
        sel_maxs = maxs(:, find(maxs(2, :) >= max(maxs(2, :) - 3)));
        wc  = sel_maxs(1, (find(sel_maxs(1, :) == min(sel_maxs(1, :)))(1))) + floor((4e-5 / Fs) * Nfft);
        wph = ceil(wc * 0.85);
        % -- Calculate D(s) and N(s); LPn:
        [filt_b, filt_a] = cheby2(filt_n, filt_attenuation, (wph / (Nfft / 2)));
        linefl_x = [1e-10 1e-10];           % This is effectively hidden.
        linefl_y = [-200 -200];             % This is effectively hidden.
        linefh_x = f(wph) * [1 1];
        linefh_y = [-100 1];
        wpl = 0;    % This is only done for further conditional check in this function.
    else
        % Basic cut frequency and BP / LP selection: -----------------------------------------------
        % -- Get the elem. where the amplitude is max and not f0 (i.e. index of the first harmonic.)
        wc = find(y == max(y));
        if length(wc) == 0
            printf("Error: unable to find first harmonic\n");
            wc = Nfft / 4;
        end
        filt_BWn = (Nfft / 2) / 10;                 % Filter bandwidth (in FFT samples).
        wpl = wc - (filt_BWn / 2);                  % Pass-band low-edge frequency (index).
        wph = wc + (filt_BWn / 2);                  % Pass-band high-edge frequency (index).

        wpl = wpl / (Nfft / 2);                     % Normalize to Nyquist range [0, 0.5].
        wph = wph / (Nfft / 2);                     % Normalize to Nyquist range [0, 0.5].

        if wpl <= 0.01   % Lower than 1% of Nfft.
            % -- Calculate D(s) and N(s); LPn:
            [filt_b, filt_a] = cheby2(filt_n, filt_attenuation, wph);
            linefl_x = [1e-10 1e-10];               % This is effectively hidden.
            linefl_y = [-200 -200];                 % This is effectively hidden.
            linefh_x = [f(ceil(wph * (Nfft / 2))) f(ceil(wph * (Nfft / 2)))];
            linefh_y = [-100 1];
        else
            % -- Calculate D(s) and N(s); BPn:
            [filt_b, filt_a] = cheby2(filt_n, filt_attenuation, [wpl wph]);
            linefl_x = [f(ceil(wpl * (Nfft / 2))) f(ceil(wpl * (Nfft / 2)))];
            linefl_y = [-100 1];
            linefh_x = [f(ceil(wph * (Nfft / 2))) f(ceil(wph * (Nfft / 2)))];
            linefh_y = [-100 1];
        end
    end
    % ----------------------------------------------------------------------------------------------

    % Get frequency response:
    [filt_h, w] = freqz(filt_b, filt_a, (Nfft / 2), "whole", Fs);

    xf = filter(filt_b, filt_a, x);                 % Apply IIR filter.
    yf = fftshift(abs(fft(xf, Nfft)));              % Get filtered FFT.
    yf = yf((Nfft / 2 + 1):end);                    % Get rid of the imaginary edge.

    % Plot the results: ----------------------------------------------------------------------------
    fig = figure(1, "visible", "off");              % Hides the figure (will be saved later).

    % -- Limits:
    fft_fmax = f(min((wc * 7), (Nfft / 2)));
    fdom_fmax = max(fft_fmax, 1e-3);
    fdom_fmin = 1e-5;

    % -- Colors:
    color_orange = [244 143 66] / 255;
    color_green  = [37 152 178] / 255;

    % -- Markers:
    ym = [max(f(wc), min(f(2:end))) y(wc)];

    % -- Frequency domain: FFT (log scale).
    subplot(3, 4, [1 2]);
    line1_x = max(f(wc), min(f(2:end))) * [1 1];
    line1_y = [-100 (20 * log10(y(wc)))];
    semilogx(f(2:end), 20 * log10(y(2:end)), "-ob", "linewidth", 0.5, "markersize", 2,
             w(2:end), 20 * log10(filt_h(2:end)), "r", "linewidth", 0.5, "markersize", 2,
             line1_x,  line1_y,  "s-k",  "markersize", 2, "linewidth", 1,
             linefl_x, linefl_y, "o--r", "markersize", 2, "linewidth", 1,
             linefh_x, linefh_y, "o--r", "markersize", 2, "linewidth", 1);
    xlabel("Frequency (Hz)", "fontsize", 7);
    ylabel("Amplitude (dB)", "fontsize", 7);

    title(["Original frequency-series for pair (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"],
        "fontsize", 9, "fontweight", "bold");

    text(ym(1), 20,
        strcat(["  1^{st} Harmonic:\n  " num2str(20 * log10(ym(2))) " dB, " num2str((1/ym(1)) / 3600) " hours"]),
        "fontsize", 6, "verticalalignment", "top");  % Adds a marker with the value

    if force_lp || wpl <= 0.01
        text(fdom_fmin, -10,
            strcat(["   Chebyshev LP" int2str(filt_n) "\n "
                "   f_c = " num2str(f(wc)) " Hz\n "
                "   T_c = " num2str((1 / f(wc)) / 60) " min." ]),
                "fontsize", 6, "color", "r", "verticalalignment", "top", "interpreter", "tex");
    else
        text(fdom_fmin, -10,
            strcat(["   Chebyshev BP" int2str(filt_n) "\n "
                "   f_c = " num2str(f(wc)) " Hz\n "
                "   T_c = " num2str((1 / f(wc)) / 60) " min." ]),
                "fontsize", 6, "color", "r", "verticalalignment", "top", "interpreter", "tex");
    end
    grid on;
    axis([fdom_fmin fdom_fmax -80 (max(20 * log10(y(2:end))) + 10)]);
    set(gca, "fontsize", 5);

    % -- Frequency domain: PSD (log scale).
    subplot(3, 4, [5 6]);
    semilogx(ff(2:end), 10 * log10(p(2:end)), "-b", "linewidth", 0.5);
    xlabel("Frequency (Hz)", "fontsize", 7);
    ylabel("PSD (dB)", "fontsize", 7);
    title(["Original Power Spectral Density for pair (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"], "fontsize", 9, "fontweight", "bold");
    grid on;
    axis([fdom_fmin fdom_fmax 0 max(10 * log10(p(find(p > 1))))]);    % Fit from 0 and p(max).
    set(gca, "fontsize", 5);

    % -- Frequency domain: FFT filtered (linear scale).
    subplot(3, 4, [3 4 7 8]);
    plot(f(2:end), (y(2:end)), "*b", "markersize", 2,
         f(2:end), (yf(2:end)), "color", color_orange, "linewidth", 1);
    xlabel("Frequency (Hz)", "fontsize", 7);
    ylabel("Amplitude", "fontsize", 7);
    title(["FFT for pair (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"], "fontsize", 9, "fontweight", "bold");
    grid on;
    axis([f(2) fdom_fmax]);
    set(gca, "fontsize", 5);
    text(ym(1), (ym(2)), strcat(["  " num2str(20 * log10(ym(2))) "dB\n  " num2str((1/ym(1)) / 3600) " hours"]), "fontsize", 7);  % Adds a marker with the value

    % -- Time-series: cross-distances:
    subplot(3, 4, [9 10]);
    plot(t, x,  "linewidth", 0.4, "b",
         t, xf, "linewidth", 0.4, "color", color_orange);
    xlabel("Time (hours)", "fontsize", 7);
    ylabel("Cross-distance (km)\nConstant component removed", "fontsize", 7);
    title(["Cross-distance vector for pair (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"], "fontsize", 9, "fontweight", "bold");
    grid on;
    axis tight;
    set(gca, "fontsize", 5);

    % -- Time-series: cross-distances (zoomed):
    subplot(3, 4, 11);
    zoom_start = length(t) - round(length(t) / 8);
    plot(t(zoom_start:end), x(zoom_start:end),  "linewidth", 0.5, "b",
         t(zoom_start:end), xf(zoom_start:end), "linewidth", 0.5, "color", color_orange);
    xlabel("Time (hours)", "fontsize", 7);
    ylabel("Cross-distance (km)\n", "fontsize", 7);
    title(["Cross-distance (zoom)"], "fontsize", 9, "fontweight", "bold");
    grid on;
    axis tight;
    set(gca, "fontsize", 5);

    % Kernel Density Estimate
    subplot(3, 4, 12);
    kxi = round(max([min(xf) -2000]));
    kxf = round(min([max(xf)  2000]));
    kx = [kxi:(kxf-kxi)/100:kxf];
    kde = kernel_density(kx', xf', 5);
    plot(kx, kde, "color", color_orange, "linewidth", 2);
    xlabel("Filtered cross-distance (km)", "fontsize", 7);
    ylabel("Estimated probability", "fontsize", 7);
    title("Kernel Density Estimation", "fontsize", 9, "fontweight", "bold");
    grid on;
    axis tight;
    set(gca, "fontsize", 5);

    % -- Save image in JPG:
    h = findall (0, "-property", "fontname");
    set(h, "fontname", "CMU Serif");
    filename = strcat(["/media/carles/VB2/plots/plots1/fig_" int2str(job_id) "_" int2str(cds_i.p(1)) "-" int2str(cds_i.p(2)) ".svg"]);
    print(fig, "-dsvg", filename, "-S1500,800");
    printf(" -- Saving plot: \x1b[32m%s\x1b[0m\n", filename);
    fflush(stdout);

endfunction
