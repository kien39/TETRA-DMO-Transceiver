# 1. Purpose: 
PlutoSDR has high frequency accuracy of 25 PPM, that may cause significant deviation of freqs impacting transmission.
For tetra at 400MHz, 25PPM may cause max 25 * 400 = 10,000 Hz. which cause off channel transmission.

Need to figure out the way to calculate freq errors / freq correction for pluto sdr.

# 2. Method:
Modulate a 5KHz sine wave to transmit at 390MHz.
Use a high freq accuracy sdr (NeSDR has 0.5PPM) to receive and demodulate to baseband. filter the signal at 2kHz, 5Khz, 8khz and compare the signal magnitudes. Use a frequency adjust range to change Rx 390Mhz + delta to see the max at 5Khz signal. then the delta is frequency errors.

