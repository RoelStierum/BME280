#include "bme280.hpp"

bme280::bme280(hwlib::i2c_bus& i2c_bus, uint8_t address) :
        i2c_bus(i2c_bus),
        address(address)
{
    control_measurement_data = static_cast<uint8_t>(OVERSAMPLING::OVER_04) | static_cast<uint8_t>(MODE::NORMAL);
}

void bme280::configure()    {
    device_id = read_dev_id_reg();
    if(
       device_id != BME280_DEVICE_ID_01)  {
        error |= static_cast<uint8_t>(bme280_ERROR::UNKNOWN_DEVICE_ID);
    }
    retrieveCalibrationData();
    setMode(MODE::NORMAL);
    if (control_measurement_data != read_ctrl_reg())   {
        error |= static_cast<uint8_t>(bme280_ERROR::UNEXPECTED_REG_DATA);
    }
    setStandbyTime(STANDBY_TIME::MS_500);
    if (config_data != read_conf_reg())   {
        error |= static_cast<uint8_t>(bme280_ERROR::UNEXPECTED_REG_DATA);
    }
    setIIR(IIR_RES::IIR_OFF);
    if (config_data != read_conf_reg())   {
        error |= static_cast<uint8_t>(bme280_ERROR::UNEXPECTED_REG_DATA);
    }
}

float bme280::getAltitude(double sea_level_pressure) {
    sea_level_pressure *= 1000;
    double temp = getTemperature() / 10;
    double pres = getPressure() * 10; // sensor pascal
    float height_in_meters = (double)(((pow(sea_level_pressure / pres, 1 / 5.257) - 1) * (temp + 273.15))) / 0.0065;
    return height_in_meters;
}

// divide by 100 to get to degrees celcius
float bme280::getTemperature()   {

    if( (control_measurement_data & 0x03 ) == static_cast<uint8_t>(MODE::FORCED) ) {
        setMode(MODE::FORCED);
    }
    i2c_bus.write(address).write(REG_PRES_DATA);
    uint8_t result_data[6];
    i2c_bus.read(address).read(result_data, 6);
    int32_t raw_temp = (((int32_t) result_data[3] << 12) | ((int32_t) result_data[4] << 4) | (int32_t) result_data[5] >> 4) << (0 - 0);
    int32_t temp_result = bme280_compensate_T_int32(raw_temp);
    if(temp_result < bme280_MIN_PRESS && temp_result > bme280_MAX_PRESS)  {
        error |= static_cast<uint8_t>(bme280_ERROR::TEMP_OUT_OF_RANGE);
    }
    return (float)temp_result / 100;
}

uint32_t bme280::getPressure()   {
    if( (control_measurement_data & 0x03 ) == static_cast<uint8_t>(MODE::FORCED) ){
        setMode(MODE::FORCED);
    }
    i2c_bus.write(address).write(REG_PRES_DATA);
    uint8_t result_data[6];
    i2c_bus.read(address).read(result_data, 6);
    int32_t raw_pressure = (((int32_t) result_data[0] << 12) | ((int32_t) result_data[1] << 4) | (int32_t) result_data[2] >> 4) << (0 - 0);
    uint32_t pres_result = bme280_compensate_P_int32(raw_pressure);
    if(pres_result < bme280_MIN_PRESS && pres_result > bme280_MAX_PRESS)  {
        error |= static_cast<uint8_t>(bme280_ERROR::PRES_OUT_OF_RANGE);
    }
    return pres_result;
}

void bme280::retrieveCalibrationData()  {
    tempSens1 = readCalibrationRegister(REG_tempSens1);
    tempSens2 = (int16_t) readCalibrationRegister(REG_tempSens2);
    tempSens3 = (int16_t) readCalibrationRegister(REG_tempSens3);
    presSens1 = readCalibrationRegister(REG_presSens1);
    presSens2 = (int16_t) readCalibrationRegister(REG_presSens2);
    presSens3 = (int16_t) readCalibrationRegister(REG_presSens3);
    presSens4 = (int16_t) readCalibrationRegister(REG_presSens4);
    presSens5 = (int16_t) readCalibrationRegister(REG_presSens5);
    presSens6 = (int16_t) readCalibrationRegister(REG_presSens6);
    presSens7 = (int16_t) readCalibrationRegister(REG_presSens7);
    presSens8 = (int16_t) readCalibrationRegister(REG_presSens8);
    presSens9 = (int16_t) readCalibrationRegister(REG_presSens9);
}

uint16_t bme280::readCalibrationRegister(const uint8_t reg_address)   {
    uint8_t dig_result[2];
    i2c_bus.write(address).write(reg_address);
    i2c_bus.read(address).read(dig_result, 2);
    unsigned short result = ((unsigned int)dig_result[1]) << 8 | (unsigned int)dig_result[0];
    return result;
}

void bme280::reset()   {
    uint8_t resetData[2] = {REG_RESET_bme, RESET_VALUE};
    i2c_bus.write(address).write(resetData, 2);
    hwlib::cout << "read_ctrl_reg reading:\t0x" << hwlib::hex << read_ctrl_reg() << "\n" << hwlib::flush;
    hwlib::cout << "Standby reading:\t0x" << hwlib::hex << read_conf_reg() << "\n" << hwlib::flush;
}

bool bme280::setOversampling(OVERSAMPLING os)  {
    uint8_t cast_os = static_cast<uint8_t>(os);
    control_measurement_data = (control_measurement_data & ~cast_os) | cast_os;
    return set_reg(REG_CTRL_MEASUREMENT, control_measurement_data);
}

bool bme280::setMode(MODE m)  {
    uint8_t cast_mode = static_cast<uint8_t>(m);
    control_measurement_data = (control_measurement_data & ~cast_mode) | cast_mode;
    return set_reg(REG_CTRL_MEASUREMENT, control_measurement_data);
}

bool bme280::setStandbyTime(STANDBY_TIME standby_time)   {
    uint8_t cast_st = static_cast<uint8_t>(standby_time);
    config_data = (config_data & ~cast_st) | cast_st;
    return set_reg(REG_CONFIG, config_data);
}

bool bme280::setIIR(IIR_RES res)   {
    uint8_t cast_res = static_cast<uint8_t>(res);
    config_data = (config_data & ~cast_res) | cast_res;
    return set_reg(REG_CONFIG, config_data);
}

bool bme280::set_reg(uint8_t reg, uint8_t val) {
    uint8_t data[] = {reg, val};
    i2c_bus.write(address).write(data, 2);
    if(reg_check)    {
        if(!(val & reg))    {
            error |= static_cast<uint8_t>(bme280_ERROR::UNEXPECTED_REG_DATA);
            return false;
        }
    }
    return true;
}

uint8_t bme280::getErrors() {
    return static_cast<uint8_t>(error);
}

uint8_t bme280::read_conf_reg()   {
    i2c_bus.write(address).write(REG_CONFIG);
    uint8_t standby_time = 0;
    i2c_bus.read(address).read(standby_time);
    return standby_time;
}

uint8_t bme280::read_ctrl_reg()  {
    i2c_bus.write(address).write(REG_CTRL_MEASUREMENT);
    uint8_t ctrl_measurement = 0;
    i2c_bus.read(address).read(ctrl_measurement);
    return ctrl_measurement;
}

uint8_t bme280::read_dev_id_reg()  {
    i2c_bus.write(address).write(REG_DEVICE_ID);
    uint8_t device_id = 0;
    i2c_bus.read(address).read(device_id);
    return device_id;
}

uint8_t bme280::getDeviceId() {
    return device_id;
}
// Code from datasheet Bosch
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
int32_t bme280::bme280_compensate_T_int32(int32_t adc_T)    {
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t) tempSens1 << 1 ))) * ((int32_t) tempSens2 )) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t) tempSens1 )) * ((adc_T >> 4 ) - ((int32_t) tempSens1))) >> 12) * ((int32_t) tempSens3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}
// Code from datasheet Bosch
// Returns pressure in Pa as unsigned 32 bit integer. Output value of “102930” equals 102930 Pa = 1029.306 hPa
uint32_t bme280::bme280_compensate_P_int32(int32_t adc_P)
{
    int32_t var1, var2;
    uint32_t p;
    var1 = (((int32_t) t_fine) >> 1) - (int32_t) 64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)presSens6);
    var2 = var2 + ((var1*((int32_t)presSens5))<<1);
    var2 = (var2>>2)+(((int32_t)presSens4)<<16);
    var1 = (((presSens3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)presSens2) * var1)>>1))>>18;
    var1 =((((32768+var1))*((int32_t)presSens1))>>15);
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;
    if (p < 0x80000000)
    {
        p = (p << 1) / ((uint32_t)var1);
    }
    else
    {
        p = (p / (uint32_t)var1) * 2;
    }
    var1 = (((int32_t)presSens9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
    var2 = (((int32_t)(p>>2)) * ((int32_t)presSens8))>>13;
    p = (uint32_t)((int32_t)p + ((var1 + var2 + presSens7) >> 4));
    return p;
}