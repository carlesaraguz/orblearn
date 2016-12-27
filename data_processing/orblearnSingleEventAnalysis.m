function orblearnSingleEventAnalysis (cds_i, job_id = 0)
% ORBLEARNSINGLEFREQANALYSIS help TBD

    x  = cds_i.d(:, 2);                     % Original distance.
    t  = cds_i.d(:, 1);                     % Original time.
    Ts = cds_i.tstep;                       % Sampling rate.
    Tu = Ts * 10;                           % Under-sampling rate.
    xu = x([1:Tu:end]);                     % Undersampled distance.
    tu = t([1:Tu:end]);                     % Undersampled time.
    tu = (tu - min(tu));                    % Start in 0.
    t  = (t  - min(t));                     % Start in 0.

    % Calculate events and encounter amplitudes: ---------------------------------------------------
    %   This process calculates time-features for each event (i.e. each encounter). These are
    %   expressed as three times: t1 is the delay between two encounters; t2 is the duration of the
    %   i-th encounter; t3 is the total sum (t1 + t2), i.e. the encounter period.
    %
    %   Encounter:   [ Ei-1 ]                     [╌╌╌ Ei ╌╌╌╌]            [ Ei+1 ]
    %   Periods:    ········ ◀─────── t1 ───────▶ ◀─── t2 ────▶ ····················
    %               ········ ◀────────────── t3 ──────────────▶ ····················
    t1 = t(2:end) - t(1:(end - 1));
    t_idx = find(t1 > cds_i.tstep);
    t1 = t1(t_idx);
    t2 = t(t_idx(1:end)) - [0; t(t_idx(1:(end - 1)) + 1)];
    t3 = t1 + t2;
    amp = zeros(length(t1) - 1, 1);
    t_idx = [0; t_idx];
    for ii = 2:length(t_idx)
        amp(ii - 1) = min(x((t_idx(ii - 1) + 1):(t_idx(ii))));
    end

    % Calculate the empirical PDF with KDE: --------------------------------------------------------
    kde_t_t1i  = min(t1) * 0.9;     kde_t_t1f  = max(t1) * 1.1;
    kde_t_t2i  = min(t2) * 0.9;     kde_t_t2f  = max(t2) * 1.1;
    kde_a_ampi = min(amp) * 0.975;  kde_a_ampf = min(max(amp) * 1.1, 1000);

    kde_interval_factor = 100;  % Amount of points in KDE interval.
    kde_bw_factor = 2;          % KDE bandwith factor; the higher the smoother.

    kde_t_t1  = [kde_t_t1i:((kde_t_t1f - kde_t_t1i) / kde_interval_factor):kde_t_t1f];
    kde_t_t2  = [kde_t_t2i:((kde_t_t2f - kde_t_t2i) / kde_interval_factor):kde_t_t2f];
    kde_a_amp = [kde_a_ampi:((kde_a_ampf - kde_a_ampi) / kde_interval_factor):kde_a_ampf];

    kde_t1_str = ["h_{KDE} = " num2str((kde_t_t1f - kde_t_t1i) / (kde_interval_factor / kde_bw_factor))];
    kde_t2_str = ["h_{KDE} = " num2str((kde_t_t2f - kde_t_t2i) / (kde_interval_factor / kde_bw_factor))];
    kde_amp_str = ["h_{KDE} = " num2str((kde_a_ampf - kde_a_ampi) / (kde_interval_factor / kde_bw_factor))];

    pdf_t1  = kernel_density(kde_t_t1', t1, (kde_t_t1f - kde_t_t1i) / (kde_interval_factor / kde_bw_factor));
    pdf_t2  = kernel_density(kde_t_t2', t2, (kde_t_t2f - kde_t_t2i) / (kde_interval_factor / kde_bw_factor));
    pdf_amp = kernel_density(kde_a_amp', amp, (kde_a_ampf - kde_a_ampi) / (kde_interval_factor / kde_bw_factor));


    % Plot the results: ----------------------------------------------------------------------------
    fig = figure(1, "visible", "off");              % Hides the figure (will be saved later).

    % -- Define colors:
    color_red    = [178  33  33] / 255;
    color_dgrey  = [102  90  90] / 255;
    color_bgrey  = [170 170 170] / 255;
    color_blue   = [ 19  95 216] / 255;
    color_orange = [229 106  18] / 255;

    subplot(4, 4, [1:2]);
    plot((tu / 3600), xu,  "linewidth", 0.4, "color", color_blue);
    xlabel("Time (hours)", "fontsize", 7);
    ylabel("Cross-distance (km)", "fontsize", 7);
    title(["Cross-distance vector for pair (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"], "fontsize", 9, "fontweight", "bold");
    grid on;
    axis tight;
    set(gca, "fontsize", 5);

    zoom_factor = 20 * length(t1) / 100;
    zoom_start = find(tu >= (tu(end) - round(tu(end) / zoom_factor)))(1);
    zoom_end   = find(tu >= (round(tu(end) / zoom_factor)))(1);

    subplot(4, 4, 3);
    plot((tu(1:zoom_end) / 3600), xu(1:zoom_end), "-s", "markersize", 2, "linewidth", 0.8, "color", color_blue);
    xlabel("Time (hours)", "fontsize", 7);
    ylabel("Cross-distance (km)", "fontsize", 7);
    title(["Cross-distance\n(beginning zoomed x" int2str(zoom_factor) ")"], "fontsize", 9, "fontweight", "bold");
    grid on;
    axis tight;
    set(gca, "fontsize", 5);

    subplot(4, 4, 4);
    plot((tu(zoom_start:end) / 3600), xu(zoom_start:end), "-s", "markersize", 2, "linewidth", 0.8, "color", color_blue);
    xlabel("Time (hours)", "fontsize", 7);
    ylabel("Cross-distance (km)", "fontsize", 7);
    title(["Cross-distance\n(ending zoomed x" int2str(zoom_factor) ")"], "fontsize", 9, "fontweight", "bold");
    grid on;
    axis tight;
    set(gca, "fontsize", 5);

    subplot(4, 4, [5:7]);
    plot(t1, "-s", "color", color_red, "markersize", 2);
    xlabel("Encounter (#)");
    ylabel("Duration (sec)");
    title(["Delay between encounters for pair (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"], "fontsize", 9, "fontweight", "bold");
    set(gca, "fontsize", 5);
    axis tight;

    subplot(4, 4, 8);
    plot(kde_t_t1, pdf_t1, "-", "color", color_red, "markersize", 2, "linewidth", 1);
    xlabel("Delay (s)");
    ylabel("P(del.)");
    title(["Delay empirical PDF (KDE)"], "fontsize", 9, "fontweight", "bold");
    set(gca, "fontsize", 5);
    axis tight;
    text(min(kde_t_t1) + (max(kde_t_t1) - min(kde_t_t1)) * 0.05, max(pdf_t1) * 0.85, kde_t1_str, "fontsize", 8);

    subplot(4, 4, [9:11]);
    plot(t2, "-s", "color", color_dgrey, "markersize", 2);
    axis([0 inf]);
    xlabel("Encounter (#)");
    ylabel("Duration (sec)");
    title(["Duration of the encounters for pair (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"], "fontsize", 9, "fontweight", "bold");
    set(gca, "fontsize", 5);
    axis tight;

    subplot(4, 4, 12);
    plot(kde_t_t2, pdf_t2, "-", "color", color_dgrey, "markersize", 2, "linewidth", 1);
    xlabel("Duration (s)");
    ylabel("P(dur.)");
    title(["Duration empirical PDF (KDE)"], "fontsize", 9, "fontweight", "bold");
    set(gca, "fontsize", 5);
    axis tight;
    text(min(kde_t_t2) + (max(kde_t_t2) - min(kde_t_t2)) * 0.05, max(pdf_t2) * 0.85, kde_t2_str, "fontsize", 8);

    subplot(4, 4, [13:15]);
    plot(amp, "-s", "color", color_orange, "markersize", 2);
    axis([0 inf]);
    xlabel("Encounter (#)");
    ylabel("Amplitude (km)");
    title(["Encounter Amplitude for pair (" int2str(cds_i.p(1)) ", " int2str(cds_i.p(2)) ")"], "fontsize", 9, "fontweight", "bold");
    set(gca, "fontsize", 5);
    axis tight;

    subplot(4, 4, 16);
    plot(kde_a_amp, pdf_amp, "-", "color", color_orange, "markersize", 2, "linewidth", 1);
    xlabel("Amplitude (km)");
    ylabel("P(amp.)");
    title(["Amplitude empirical PDF (KDE)"], "fontsize", 9, "fontweight", "bold");
    set(gca, "fontsize", 5);
    axis tight;
    text(min(kde_a_amp) + (max(kde_a_amp) - min(kde_a_amp)) * 0.05, max(pdf_amp) * 0.85, kde_amp_str, "fontsize", 8);

    % -- Save image in SVG:
    filename = strcat(["/media/carles/VB2/plots/plots3/fig_" int2str(job_id) "_" int2str(cds_i.p(1)) "-" int2str(cds_i.p(2)) ".svg"]);
    % filename = strcat(["/home/carles/Development/orblearn/data_processing/plots3/fig_" int2str(job_id) "_" int2str(cds_i.p(1)) "-" int2str(cds_i.p(2)) ".svg"]);
    print(fig, "-dsvg", filename, "-S1400,800");
    printf(" -- Saving plot: \x1b[32m%s\x1b[0m\n", filename);
    fflush(stdout);

endfunction
