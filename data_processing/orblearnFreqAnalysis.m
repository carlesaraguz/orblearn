%% Reset environment -------------------------------------------------------------------------------
clear;
close all;
clc;

graphics_toolkit("gnuplot");

%% Load propagation files and calculate cross-distances --------------------------------------------
[cds, items] = orblearnLoad3("../propagations/historic1/", 10000000, 100);

Ts = 60;        % Known sampling period (i.e. propagation step).
Fs = 1 / Ts;    % Known sampling frequency.

for ii = 1:items
    Nfft = 2 ** (nextpow2(length(cds(ii).d)) - 3);   % Number of FFT samples.

    % Remove f(0) component and apply cut at a maximum distance:
    x = (cds(ii).d(:, 2)) .* (cds(ii).d(:, 2) < 1000);
    Af0 = sum(x) / length(x);
    x = x - Af0;

    y = fftshift(abs(fft(x, Nfft)));
    [p, ff] = periodogram(x, ones(size(x)), Nfft, Fs);
    x_win = round(length(x)/3);
    % [p1, ff1] = periodogram(x((        1):(x_win * 1)), ones(size(x((        1):(x_win * 1)))), Nfft, Fs);
    % [p2, ff2] = periodogram(x((x_win * 1):(x_win * 2)), ones(size(x((x_win * 1):(x_win * 2)))), Nfft, Fs);
    % [p3, ff3] = periodogram(x((x_win * 2):(end      )), ones(size(x((x_win * 2):(end      )))), Nfft, Fs);

    % Plot the results:
    fig = figure(1, "visible", "off");      % Hides the figure (will be saved in JPG).

    % -- Plot's axis:
    f = (Fs * (-Nfft/2:(Nfft/2 - 1)) / Nfft);               % Frequency axis (in mHz).
    t = (cds(ii).d(:, 1) - min(cds(ii).d(:, 1))) / 3600;    % Time axis (in hours).
    fft_fmax = f(find(y == max(y)))(length(f(find(y == max(y))))) * 7;
    if fft_fmax == 0
        fft_fmax = 1 / (3600 * 72);
    end

    % -- Markers:
    ym = [f(find(y == max(y)))(length(f(find(y == max(y))))) max(y)];
    pm = [ff(find(p == max(p))) max(p)];

    % -- Time-series: cross-distances.
    subplot(2, 4, [1 2 3]);
    plot(t, x, "linewidth", 0.5, "color", [58/255 168/255 93/255]);
    xlabel("Time (hours)", "fontsize", 7);
    ylabel("Cross-distance (km)\nConstant component removed", "fontsize", 7);
    title(["Cross-distance vector for pair (" int2str(cds(ii).p(1)) ", " int2str(cds(ii).p(2)) ")"], "fontsize", 9, "fontweight", "bold");
    grid on;
    grid minor on;
    axis tight;
    set(gca, "fontsize", 5);

    % -- Frequency domain: FFT (linear).
    subplot(2, 4, [5 6]);
    plot(f(find(f > 0)), y(find(f > 0)), "-ob", "linewidth", 0.5, "markersize", 2);
    xlabel("Frequency (Hz)", "fontsize", 7);
    ylabel("|FFT(x)|", "fontsize", 7);
    title(["FFT for pair (" int2str(cds(ii).p(1)) ", " int2str(cds(ii).p(2)) ")"], "fontsize", 9, "fontweight", "bold");
    grid on;
    axis([0 fft_fmax]);       % Plots abs(FFT) from f0 to 2·F(y_max).
    set(gca, "fontsize", 5);
    text(ym(1), ym(2), strcat([num2str(ym(2)) "\n" num2str((1/ym(1)) / 3600) " hours"]), "fontsize", 7);  % Adds a marker with the value

    % -- Frequency domain: PSD (dB).
    subplot(2, 4, [7 8]);
    % hold on;
    semilogx(ff(find(p > 1)), 10 * log10(p(find(p > 1))), "-r", "linewidth", 0.5);
    % semilogx(ff1(find(p1 > 1)), 10 * log10(p1(find(p1 > 1))), "-m", "linewidth", 0.1);
    % semilogx(ff2(find(p2 > 1)), 10 * log10(p2(find(p2 > 1))), "-m", "linewidth", 0.1);
    % semilogx(ff3(find(p3 > 1)), 10 * log10(p3(find(p3 > 1))), "-m", "linewidth", 0.1);
    xlabel("Frequency (Hz)", "fontsize", 7);
    ylabel("PSD (dB)", "fontsize", 7);
    title(["Power Spectral Density for pair (" int2str(cds(ii).p(1)) ", " int2str(cds(ii).p(2)) ")"], "fontsize", 9, "fontweight", "bold");
    grid on;
    axis([1e-5 max(fft_fmax, 1e-3) 0 max(10 * log10(p(find(p > 1))))]);    % Fit from 0 and p(max).
    set(gca, "fontsize", 5);


    % Kernel Density Estimate
    x = (cds(ii).d(:, 2)) .* (cds(ii).d(:, 2) < 1000);  % Get original signal (with f0 component).
    x = x(find(x != 0));                                % Remove the zeroes.

    subplot(2, 4, 4);
    kx = [0:1000]';
    kde = kernel_density(kx, x, 20);
    plot(kx, kde);
    xlabel("Cross-distance (km)", "fontsize", 7);
    ylabel("Estimated probability", "fontsize", 7);
    title("Kernel Density Estimation", "fontsize", 9, "fontweight", "bold");
    grid on;
    set(gca, "fontsize", 5);

    % -- Save image in JPG:
    h = findall (0, "-property", "fontname");
    set(h, "fontname", "CMU Serif");
    filename = strcat(["plots1/fig_" int2str(ii) "_" int2str(cds(ii).p(1)) "-" int2str(cds(ii).p(2)) ".svg"]);
    print(fig, "-dsvg", filename, "-S1000,500");
    printf("Saving plot 1: %s\n", filename);
    fflush(stdout);

end
