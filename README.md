# 24GHz-Radar for Movement Detection

A 24 GHz radar sensor detects movements and sends the detected signal to the bandpass filter.
The 4th order Bandpass filter gives an amplified signal as the output which in turn is sent to the ADC.
PSOC calculates the FFT of ADC data and sends it to MATLAB.
CFAR is used to set a threshold with which the probability of a false alarm is reduced.
