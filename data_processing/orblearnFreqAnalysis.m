%% Reset environment -------------------------------------------------------------------------------
clear;
close all;
clc;

graphics_toolkit("gnuplot");

%% Load propagation files and calculate cross-distances --------------------------------------------
[cds, items] = orblearnLoad3("../propagations/historic1/", 10000000, 20);

Ts = 60;        % Known sampling period (i.e. propagation step).
Fs = 1 / Ts;    % Known sampling frequency.

for ii = 1:items
    Nfft = 2 ** nextpow2(length(cds(ii).d));   % Number of FFT samples.

    % Remove f(0) component and apply cut at a maximum distance:
    x = (cds(ii).d(:, 2)) .* (cds(ii).d(:, 2) < 1000);
    Af0 = sum(x) / length(x);
    x = x - Af0;

    y = fftshift(abs(fft(x, Nfft)));
    [p, ff] = periodogram(x, ones(size(x)), Nfft, Fs);


    % Plot the results:
    fig = figure(1, "visible", "off");      % Hides the figure (will be saved in JPG).

    % -- Plot's axis:
    f = (Fs * (-Nfft/2:(Nfft/2 - 1)) / Nfft);               % Frequency axis (in mHz).
    t = (cds(ii).d(:, 1) - min(cds(ii).d(:, 1))) / 3600;    % Time axis (in hours).
    fft_fmax = (f(find(y == max(y)))(2)) * 4;

    % -- Markers:
    ym = [f(find(y == max(y))(2)) max(y)];
    pm = [ff(find(p == max(p))) max(p)];

    % -- Time-series: cross-distances.
    subplot(2, 4, [1 2 3 4]);
    plot(t, x, "linewidth", 0.5, "color", [58/255 168/255 93/255]);
    xlabel("Time (hours)", "fontsize", 8);
    ylabel("Cross-distance (km)\nConstant component removed", "fontsize", 8);
    title(["Cross-distance vector for pair (" int2str(cds(ii).p(1)) ", " int2str(cds(ii).p(2)) ")"], "fontsize", 11, "fontweight", "bold");
    grid on;
    grid minor on;
    axis tight;

    % -- Frequency domain: FFT (linear).
    subplot(2, 4, [5 6]);
    plot(f(find(f > 0)), y(find(f > 0)), "-ob", "linewidth", 0.5);
    xlabel("Frequency (Hz)", "fontsize", 8);
    ylabel("|FFT(x)|", "fontsize", 8);
    title(["FFT for pair (" int2str(cds(ii).p(1)) ", " int2str(cds(ii).p(2)) ")"], "fontsize", 11, "fontweight", "bold");
    grid on;
    axis([0 fft_fmax]);       % Plots abs(FFT) from f0 to 2·F(y_max).
    text(ym(1), ym(2), strcat([" " num2str(ym(2)) "\n " num2str((1/ym(1)) / 3600) " hours"]));  % Adds a marker with the value

    % -- Frequency domain: PSD (dB).
    subplot(2, 4, [7 8]);
    semilogx(ff(find(p > 1)), 10 * log10(p(find(p > 1))), "-r", "linewidth", 0.5);
    xlabel("Frequency (Hz)", "fontsize", 8);
    ylabel("PSD (dB)", "fontsize", 8);
    title(["Power Spectral Density for pair (" int2str(cds(ii).p(1)) ", " int2str(cds(ii).p(2)) ")"], "fontsize", 11, "fontweight", "bold");
    grid on;
    axis([1e-5 fft_fmax 0 max(10 * log10(p(find(p > 1))))]);    % Fit from 0 and p(max).

    % -- Save image in JPG:
    filename = strcat(["plots1/fig_" int2str(ii) ".jpg"]);
    print(fig, "-djpg", filename, "-S1200,700");
    printf("%s\n", filename);
    fflush(stdout);
end
