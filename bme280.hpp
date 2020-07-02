/**
    This file contains all the requirements to use the bme280 class.


    * \file bme280.hpp
    * \author Roel Stierum
*/
#ifndef _BME280V2_
#define _BME280V2_
#include "hwlib.hpp"

#include <math.h>

// Minimum and maximum temperature for bme280 according to the datasheet
static constexpr int8_t bme280_MIN_TEMP = -40;
static constexpr int8_t bme280_MAX_TEMP = 85;


// Minimum and maximum pressure for bme280 according to the datasheet
static constexpr uint16_t bme280_MIN_PRESS = 300;
static constexpr uint16_t bme280_MAX_PRESS = 1100;


// Known device id's for BME280
static constexpr uint8_t bme280_DEVICE_ID_01 = 0x57;
static constexpr uint8_t bme280_DEVICE_ID_02 = 0x58;
static constexpr uint8_t bme280_DEVICE_ID_03 = 0x59;
static constexpr uint8_t BME280_DEVICE_ID_01 = 0x60;

// bme280 registers that are used
static constexpr uint8_t REG_DEVICE_ID = 0xD0;
static constexpr uint8_t REG_RESET_bme = 0xE0;
static constexpr uint8_t REG_CTRL_MEASUREMENT = 0xF4;
static constexpr uint8_t REG_CONFIG = 0xF5;
static constexpr uint8_t REG_PRES_DATA = 0xF7;
static constexpr uint8_t REG_TEMP_DATA = 0xFA;
static constexpr uint8_t RESET_VALUE = 0xB6;

// Registers for calibration data (provided by bosch)
static constexpr uint8_t REG_tempSens1 = 0x88;
static constexpr uint8_t REG_tempSens2 = 0x8A;
static constexpr uint8_t REG_tempSens3 = 0x8C;
static constexpr uint8_t REG_presSens1 = 0x8E;
static constexpr uint8_t REG_presSens2 = 0x90;
static constexpr uint8_t REG_presSens3 = 0x92;
static constexpr uint8_t REG_presSens4 = 0x94;
static constexpr uint8_t REG_presSens5 = 0x96;
static constexpr uint8_t REG_presSens6 = 0x98;
static constexpr uint8_t REG_presSens7 = 0x9A;
static constexpr uint8_t REG_presSens8 = 0x9C;
static constexpr uint8_t REG_presSens9 = 0x9E;

static constexpr uint8_t REG_DIG_RESERVED = 0xA0;

static constexpr uint8_t STANDBY_TIME_500_US = 0x00;
static constexpr uint8_t STANDBY_TIME_62500_US = 0x01;
static constexpr uint8_t STANDBY_TIME_125_MS = 0x02;
static constexpr uint8_t STANDBY_TIME_250_MS = 0x03;
static constexpr uint8_t STANDBY_TIME_500_MS = 0x04;
static constexpr uint8_t STANDBY_TIME_1_S = 0x05;
static constexpr uint8_t STANDBY_TIME_2_S = 0x06;
static constexpr uint8_t STANDBY_TIME_4_S = 0x07;

/**
    \enum STANDBY_TIME
    \brief Enum for standby time values between measurements taken by bme280
    (only when bme280 is in normal mode).
*/
enum class STANDBY_TIME : uint8_t{
    US_500 = 0x00,  /**< 500 microseconds */
    US_62500 = 0x20,    /**< 62.5 milliseconds */
    MS_125 = 0x40,  /**< 125 milliseconds */
    MS_250 = 0x60,  /**< 250 milliseconds */
    MS_500 = 0x80,  /**< 500 milliseconds */
    S_1 = 0xA0, /**< 1 second */
    S_2 = 0xC0, /**< 2 seconds */
    S_4 = 0xE0, /**< 4 seconds */

    BME280_US_500   = 0b00000000, /**< BME280 500 microseconds */
    BME280_MS_10    = 0b11000000, /**< BME280 10 milliseconds */
    BME280_MS_20    = 0b11100000, /**< BME280 20 milliseconds */
    BME280_US_62500 = 0b00100000, /**< BME280 62.5 milliseconds */
    BME280_MS_125   = 0b01000000, /**< BME280 125 milliseconds */
    BME280_MS_250   = 0b01100000, /**< BME280 250 milliseconds */
    BME280_MS_500   = 0b10000000, /**< BME280 500 milliseconds */
    BME280_S_1      = 0b10100000, /**< BME280 1 second */

};

/**
    \enum IIR_RES
    \brief Enum for IIR resolution. used for changing rate of change of pressure measurement based on past measurements.
*/
enum class IIR_RES : uint8_t{
    IIR_OFF = 0x00, /**< IIR Res off */
    IIR_02 = 0b00000100,  /**< IIR Res x2 */
    IIR_04 = 0b00001000,  /**< IIR Res x4 */
    IIR_08 = 0b00001100,  /**< IIR Res x8 */
    IIR_16 = 0b00010000,  /**< IIR Res x16 */
};

/**
    \enum PRES_OVERSAMPLING
    \brief Enum for Pressure sampling setting.
*/
enum class PRES_OVERSAMPLING : uint8_t{
    PRES_OS_OFF = 0x00, /**< Temperature measurement off */
    PRES_OS_01 = 0x04,  /**< Pressure measurement 1   (2.62 Pa) */
    PRES_OS_02 = 0x08,  /**< Pressure measurement 2   (1.31 Pa) */
    PRES_OS_04 = 0x0B,  /**< Pressure measurement 4   (0.66 Pa) */
    PRES_OS_08 = 0x10,  /**< Pressure measurement 8   (0.33 Pa) */
    PRES_OS_16 = 0x14,  /**< Pressure measurement 16  (0.16 Pa) */
};

/**
    \enum TEMP_OVERSAMPLING
    \brief Enum for temperature sampling setting.
*/
enum class TEMP_OVERSAMPLING : uint8_t{
    TEMP_OS_OFF = 0x00, /**< Temperature measurement off */
    TEMP_OS_01 = 0x20,  /**< Temperature measurement 1  (0.00050 Degrees Celcius) */
    TEMP_OS_02 = 0x40,  /**< Temperature measurement 2  (0.00025 Degrees Celcius) */
    TEMP_OS_04 = 0x50,  /**< Temperature measurement 4  (0.00012 Degrees Celcius) */
    TEMP_OS_08 = 0x80,  /**< Temperature measurement 8  (0.00006 Degrees Celcius) */
    TEMP_OS_16 = 0xA0,  /**< Temperature measurement 16 (0.00003 Degrees Celcius) */
};

/**
    \enum OVERSAMPLING
    \brief Enum for Oversampling. Combines both temperature and pressure oversampling into a single value (mask).
    \author Roel Stierum
*/
enum class OVERSAMPLING : uint8_t   {
    OVER_01 = static_cast<uint8_t>(PRES_OVERSAMPLING::PRES_OS_01) | static_cast<uint8_t>(TEMP_OVERSAMPLING::TEMP_OS_01),
    OVER_02 = static_cast<uint8_t>(PRES_OVERSAMPLING::PRES_OS_02) | static_cast<uint8_t>(TEMP_OVERSAMPLING::TEMP_OS_02),
    OVER_04 = static_cast<uint8_t>(PRES_OVERSAMPLING::PRES_OS_04) | static_cast<uint8_t>(TEMP_OVERSAMPLING::TEMP_OS_04),
    OVER_08 = static_cast<uint8_t>(PRES_OVERSAMPLING::PRES_OS_08) | static_cast<uint8_t>(TEMP_OVERSAMPLING::TEMP_OS_08),
    OVER_16 = static_cast<uint8_t>(PRES_OVERSAMPLING::PRES_OS_16) | static_cast<uint8_t>(TEMP_OVERSAMPLING::TEMP_OS_16),
};

/**
    \enum MODE
    \brief Enum to select mode for bme280 to operate in.
    \author Roel Stierum
*/
enum class MODE : uint8_t {
    SLEEP = 0x00, /**< bme280 in sleep mode, won't take measurements */
    FORCED = 0x01,  /**< bme280 will only take a measurement when temperature or pressure is requested */
    NORMAL = 0b00000011, /**< bme280 will run in continuous mode with a standby between every measurement */
};

// enum for testing for various errors.
enum class bme280_ERROR : uint8_t{
    NO_ERROR = 0x00,
    UNKNOWN_DEVICE_ID = 0x01,
    UNEXPECTED_REG_DATA = (0x01) << 1,
    TEMP_OUT_OF_RANGE = (0x01) << 2,
    PRES_OUT_OF_RANGE = (0x01) << 3,
};

// Helper function to print floats because hwlib doesn't allow me to do so
inline hwlib::ostream & operator<< ( hwlib::ostream & stream, const float & f ) {
    int i_value = (int)f;

    float f_value = f - (float)i_value;
    f_value *= pow(10, 2);
    stream << i_value << '.' << (unsigned int)f_value;

    return stream;
}

/**
    \brief  This class provides the usage of the bme280 in 3 modes (SLEEP, FORCED, STANDBY).
    \author Roel Stierum
    \version 1.0.0
*/
class bme280 {
public:
    /**
        \brief Constructor of bme280.
        Requires a i2c bus to use for communication with the bme280 and the i2c address,
        (usually 0x76 | 0x77) of the bme280.
        Default values that are set for the bme280 are normal mode and oversampling x2.
        This constructor does not perform any read or write operations with the bme280.
        \param[in] hwlib::i2c_bus& i2c_bus The hwlib i2c bus to use.
        \param[in] uint8_t Address The i2c address for the bme280.
    */
    bme280(hwlib::i2c_bus& i2c_bus, uint8_t address);

    /**
        \brief Configures the bme280 by retrieving required data from the bme280 and setting register values.
        Performs read and write operations to the bme280.
        \warning Retrieving temperature and pressure before calling this method will result in undefined behaviour (wrong measurements).
    */
    void configure();

    /**
        \brief Get the temperature in degrees celcius .
        Performs a read operation to the bme280.
        \details returns float Degrees celcius of measured temperature.
    */
    float getTemperature();

    /**
        \brief Get the pressure in pA.
        Performs a read operation to the bme280.
        \details returns uint32_t Pressure in unit of pA.
    */
    uint32_t getPressure();

    /**
        \brief Helper method that gets the altitude based on the temperature and pressure. The return value is expressed in meters.
        Performs write operations to the bme280.
        \param[in] sea_level_pressure The local pressure at sea level, expressed in hectopascal (hPa).
        \details returns float Altitude in meters.
    */
    float getAltitude(double sea_level_pressure = 1013.15);

    /**
        \brief Resets the bme280 to factory default.
        Performs a write operation to the bme280.
    */
    void reset();

    /**
        \brief Sets the oversampling for obtaining measurements, Default is 2.
        Performs a read and write operation to the bme280.
        \param[in] OVERSAMPLING The oversampling to use.
        \details bool Returns true when value has been set succesfully and false if failed.
    */
    bool setOversampling(OVERSAMPLING os);

    /**
        \brief Mode that the bme280 should operate in. Default is normal mode.
        Performs a read and write operation to the bme280.
        \param[in] MODE Mode that the bme280 should operate in.
        \details bool Returns true when value has been set succesfully and false if failed.
    */
    bool setMode(MODE m);

    /**
        \brief Sets the filtering coefficient for pressure.
        Performs a read and write operation to the bme280.
        \param[in] IIR_RES resolution of the filter.
        \details bool Returns true when value has been set succesfully and false if failed.
    */
    bool setIIR(IIR_RES res);

    /**
        \brief Set the standby time beween measurements when in normal mode.
        Performs a read and write operation to the bme280.
        \param[in] STANDBY_TIME standby time enum for standby time between measurements.
        \details bool Returns true when value has been set succesfully and false if failed.
    */
    bool setStandbyTime(STANDBY_TIME standby_time);

    /**
        \brief Gets the device id that has been found for the bme.
        \details  uint8_t The device id (not i2c address!) for the bme280 at address 0xD0.
    */
    uint8_t getDeviceId();

    /**
        \brief Returns uint8_t that can contain error types.
        \details uint8_t Error types of error that have been found.
    */
    uint8_t getErrors();
private:
    /**
        \brief Sets register data

    */
    bool set_reg(uint8_t reg, uint8_t val);

    /**
        \brief Reads control register data
    */
    uint8_t read_ctrl_reg();

    /**
        \brief Reads configuration register data
    */

    uint8_t read_conf_reg();

    /**
        \brief Reads device ID register data
    */
    uint8_t read_dev_id_reg();

    /**
           \brief Retrieves all the calibration data from the calibration registers.
       */
    void retrieveCalibrationData();

    /**
           \brief Retrieves calibration data of a single calibration register.
       */
    uint16_t readCalibrationRegister(const uint8_t reg_address);

    /**
           \Brief Returns temperature as degrees celcius * 100
       */
    int32_t bme280_compensate_T_int32(int32_t adc_T);

    /**
           \brief Returns pressure in Pa as unsigned 32 bit integer. Output value of “102930” equals 102930 Pa = 1029.306 hPa
       */

    uint32_t bme280_compensate_P_int32(int32_t adc_P);

    /**
           \brief The i2c bus that is used to communicate with the bme280.
       */
    hwlib::i2c_bus& i2c_bus;

    /**
           \brief The i2c Address of the bme280
       */
    uint8_t address;

    int32_t t_fine;

    /**
           \brief Internal id of the bme280
       */

    uint8_t device_id = 0;

    /**
           \brief Calibration values for calculating temperature.
       */
    uint16_t    tempSens1;
    int16_t     tempSens2, tempSens3;

    /**
           \brief Calibration values for calculating temperature.
       */
    uint16_t    presSens1;
    int16_t     presSens2, presSens3, presSens4, presSens5, presSens6, presSens7, presSens8, presSens9;


    uint8_t control_measurement_data = 0x00;
    uint8_t config_data = 0x00;
    uint8_t error = 0x00;
    const bool reg_check = true;
};
#endif // _BME280V2_