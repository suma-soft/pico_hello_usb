#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "bme280_port.h"

void test_bme_read() {
    struct bme280_data comp_data;
    int8_t res = bme280_read_data(&comp_data);

    if (res == BME280_OK) {
        printf("🌡️ Temp: %.2f °C | 💧 Humidity: %.2f %% | ⬇️ Pressure: %.2f hPa\n",
               comp_data.temperature,
               comp_data.humidity,
               comp_data.pressure / 100.0f);
    } else {
        printf("❌ Błąd odczytu danych z BME280: %d\n", res);
    }
}

int main() {
    stdio_init_all();
    setvbuf(stdout, NULL, _IONBF, 0);  // wyłączenie buforowania
    printf("✅ Pico gotowe – czekam na Putty...\n");

    // ⏳ Poczekaj 30 sekund przed inicjalizacją BME
    sleep_ms(30000);

    printf("➡️ Rozpoczynam inicjalizację BME280...\n");

    int8_t status = bme280_init_default();
    if (status == BME280_OK) {
        printf("✅ BME280 zainicjalizowany poprawnie\n");
    } else {
        printf("❌ Błąd inicjalizacji BME280: %d\n", status);
        return 1;
    }

    while (1) {
        test_bme_read();
        sleep_ms(2000);
    }

    return 0;
}
