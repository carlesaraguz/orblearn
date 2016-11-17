%% Reset environment -------------------------------------------------------------------------------
clear;
close all;
clc;

pkg load image;

%% Load propagation files --------------------------------------------------------------------------
[Pcurr, icurr] = orblearnLoad2("propagations/compare_current/");
[Phist, ihist] = orblearnLoad2("propagations/compare_historic/");

t = Pcurr(:, 1);
t = (t - t(1)) / 3600;  % Time in hours.
latcurr = Pcurr(:, 2);
lngcurr = Pcurr(:, 3);
xcurr = Pcurr(:, 4);
ycurr = Pcurr(:, 5);
zcurr = Pcurr(:, 6);
lathist = Phist(:, 2);
lnghist = Phist(:, 3);
xhist = Phist(:, 4);
yhist = Phist(:, 5);
zhist = Phist(:, 6);

D = sqrt(sum((Pcurr(:, 4:6) - Phist(:, 4:6)).^2, 2));

subplot(6, 4, [14 15 16 18 19 20 22 23 24]);
plot(t, D);
grid on
title("Absolute distance (km)");
xlabel("time (h)");
ylabel("Distance (km)");

subplot(6, 4, [1 5]);
plot(t, xcurr - xhist);
grid on
title("X-distance (km)");
xlabel("time (h)");
ylabel("Distance (km)");

subplot(6, 4, [9 13]);
plot(t, ycurr - yhist);
grid on
title("Y-distance (km)");
xlabel("time (h)");
ylabel("Distance (km)");

subplot(6, 4, [17 21]);
plot(t, zcurr - zhist);
grid on
title("Z-distance (km)");
xlabel("time (h)");
ylabel("Distance (km)");

subplot(6, 4, [2 3 4 6 7 8 10 11 12]);
imgbg = imread("earth_map.png");
imgbg = imresize(imgbg, [180 360]);
image(imgbg);
hold on
T = icurr(3);
plot_start = size(t, 1) - (255 * 60 / T);
plot_end   = size(t, 1) - ((255 - 93) * 60 / T);
plot((lngcurr(plot_start:plot_end) + 180), (latcurr(plot_start:plot_end) + 90), "linewidth", 1, "color", "g", "marker", "s");
hold on
plot((lnghist(plot_start:plot_end) + 180), (lathist(plot_start:plot_end) + 90), "linewidth", 1, "color", "r", "marker", "p");
xlim([0 360])
ylim([0 180])
grid on

print -djpg "orbdiff.jpg"
