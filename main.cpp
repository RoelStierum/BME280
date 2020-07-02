#include "hwlib.hpp"
#include "array"
#include "bme280.hpp"
int main( void ){

    namespace target = hwlib::target;
    hwlib::wait_ms(250);
    auto scl           = hwlib::target::pin_oc{ hwlib::target::pins::scl };
    auto sda           = hwlib::target::pin_oc{ hwlib::target::pins::sda };
    auto i2c_bus       = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );
    auto font          = hwlib::font_default_8x8();
    auto oled          = hwlib::glcd_oled( i2c_bus );
    auto display       = hwlib::terminal_from( oled, font );
    hwlib::wait_ms( 1'000 );
    bme280 chiptest(i2c_bus, 0x76);
    chiptest.configure();
    chiptest.setIIR(IIR_RES::IIR_08);
    float bodytemp = 37.0;


    // values after getAltitude must be filled in with the current air pressure.

    while(1) {

        //loop for pushing out sensor values to the terminal edit
        hwlib::wait_ms(1500);
        hwlib::cout << "\n" << "Temperature = " << hwlib::dec << chiptest.getTemperature() << "\n";
        hwlib::cout << "Pressure = " << hwlib::dec << chiptest.getPressure() / 100 << "\n";
        hwlib::cout << "Altitude = " << hwlib::dec << chiptest.getAltitude(1016.2) << " meter\n";
        if(chiptest.getTemperature() < bodytemp){
            display << "\n" << "You don't have covid!";
        }else{
            display << "\n" << "You have to see a doctor!";
        }

        //to push sensor values to oled
      display
      << "\f" << "Temp: " << chiptest.getTemperature();
        if(chiptest.getTemperature() < bodytemp){
            display << "\n" << "You don't have" "\n" << "covid-19!";
        }else{
            display << "\n" << "You have to see" "\n" <<  "a doctor!";
        }
      display << "\n" << "Pres: " << chiptest.getPressure() / 100;
      display << "\n" << "Alt:  " << chiptest.getAltitude(1016.2)
      << hwlib::flush;
        hwlib::wait_ms(100);
    }
}

