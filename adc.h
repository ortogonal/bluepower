#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/**
 * Reads the raw value from the selected ADC channel
 * @param address - I2C address for the target device e.g. 0x68
 * @param channel - 1 to 4
 * @param bitrate - 12, 14, 16 or 18
 * @param pga - 1, 2, 4 or 8
 * @param conversionmode - 0 = one shot conversion, 1 = continuous conversion
 * @returns - raw long value from ADC buffer
 */
long read_raw(char address, char channel, int bitrate, int pga,	char conversionmode);


/**
* Returns the voltage from the selected ADC channel
* @param address - I2C address for the target device e.g. 0x68
* @param channel - 1 to 4
* @param bitrate - 12, 14, 16 or 18
* @param pga - 1, 2, 4 or 8
* @param conversionmode - 0 = one shot conversion, 1 = continuous conversion
* @returns - double voltage value from ADC
*/
double read_voltage(char address, char channel, int bitrate, int pga, char conversionmode);

#ifdef __cplusplus
}
#endif // __cplusplus

