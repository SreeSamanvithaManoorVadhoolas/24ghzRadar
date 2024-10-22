% Initialize parameters
N = 1024;     % Number of samples
NG = 2;       % Number of Guard Cells
NR = 6;       % Number of Reference Cells
PFA = 10^-6;  % Probability of False Alarm

% Load data and compute FFT
load('CW_rx_data_adc_10.mat')
rx_fft_data = fft(rx_data_adc);
abs_fft_data = abs(rx_fft_data);

% Initialize threshold and detection arrays
threshold = zeros(1, N);
detection = zeros(1, N);

for i = 1:N
    if i > NG + NR && i <= N - (NG + NR)
        % Define reference cell indices for the main window
        referenceWindow = [abs_fft_data(i - NR - NG:i - NG - 1), abs_fft_data(i + NG + 1:i + NR + NG)];
    else
        % Handle edge cases
        if i <= NG + NR
            % Start edge case
            referenceWindow = abs_fft_data([1:i - NG - 1, i + NG + 1:NR + NG + i]);
        else
            % End edge case
            referenceWindow = abs_fft_data([i - NR - NG:i - NG - 1, i + NG + 1:N]);
        end
    end
    noise_power = mean(referenceWindow.^2);

    % Calculate the threshold
    if ~isempty(referenceWindow) && all(noise_power > 0)
        thresholdFactor = (PFA^(-1/length(referenceWindow)) - 1) * length(referenceWindow);
        threshold(i) = thresholdFactor * mean(noise_power);
        % Detection logic
        detection(i) = abs_fft_data(i)^2 > threshold(i);
    else
        threshold(i) = Inf; % No detection possible
        detection(i) = 0;
    end
end


% Plot results
figure;
subplot(2, 1, 1);
plot(0:N-1, 20 * log10(abs(rx_fft_data)), 'b');
hold on;
plot(0:N-1, 10 * log10(threshold), 'g');
title('FFT Magnitude and CFAR Threshold');
xlabel('Frequency Bin');
ylabel('Magnitude (dB)');
legend('FFT Magnitude', 'CFAR Threshold');

subplot(2, 1, 2);
plot(0:N-1, detection, 'r');
title('CFAR Detection');
xlabel('Frequency Bin');
ylabel('Detection');
legend('Detection');
