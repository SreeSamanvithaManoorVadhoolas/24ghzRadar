% Clear previous variables and close figures
close all;
clear all;
clc;

% Parameters
ADC_bytes = 1024 * 2;
FFT_bytes = 2048 * 4;

% Serial communication setup
priorPorts = instrfind;
delete(priorPorts);
PSoC = serial('COM7', 'BaudRate', 115200, 'InputBufferSize', 40480);
fopen(PSoC);

% Figure for plotting
f1 = figure;
count = 1;

% Flag to check data availability
flg_data_avail = 0;
fwrite(PSoC, 's', 'uchar'); % Ready to receive
while(flg_data_avail == 0)
    if PSoC.BytesAvailable >= ADC_bytes + FFT_bytes
        fwrite(PSoC, 'o', 'uchar'); % Acknowledge reception
        rx_data_adc = fread(PSoC, 1024, 'uint16');
        rx_data_fft = fread(PSoC, 2048, 'int32');
        fprintf(" Transfer %i DONE \n", count);

        % Separate real and imaginary parts of FFT data
        fft_real = rx_data_fft(1:2:end);
        fft_imag = rx_data_fft(2:2:end);
        fft_mag = sqrt(fft_real.^2 + fft_imag.^2);

        % Plotting the received data
        subplot(3, 1, 1)
        plot(0:(length(rx_data_adc)-1), rx_data_adc(1:length(rx_data_adc)));
        title(['Received Time Domain Data No.:', num2str(count)]);

        subplot(3, 1, 2)
        plot(0:1023, 1/length(rx_data_adc) * 20 * log10(abs(fft(rx_data_adc))));
        title('FFT - Array - Matlab');

        subplot(3, 1, 3)
        plot(0:1023, 1/length(rx_data_adc) * 20 * log10(fft_mag));
        title('FFT - Array - PSoC');

        % Save the received data
        save(['CW_rx_data_adc_', num2str(count), '.mat'], 'rx_data_adc');
        count = count + 1;
    end

    if count == 11
        break;
    end

    fwrite(PSoC, 's', 'uchar'); % Ready to receive
end

fclose(PSoC);
fprintf(" Scipt End \n");