#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "bme280_port.h"

#define I2C_PORT i2c0
#define SDA_PIN 6
#define SCL_PIN 7
#define BME280_ADDR 0x77  // lub 0x76 w zależności od modułu

struct bme280_dev bme;
static uint8_t dev_addr = BME280_ADDR;

static int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
    uint8_t addr = *(uint8_t *)intf_ptr;
    printf("📥 [I2C READ] addr 0x%02X, reg 0x%02X, len %lu\n", addr, reg_addr, len);

    int write_res = i2c_write_blocking(I2C_PORT, addr, &reg_addr, 1, true);
    if (write_res < 0) {
        printf("❌ i2c_write_blocking failed with code %d\n", write_res);
        return BME280_E_COMM_FAIL;
    }

    int read_res = i2c_read_blocking(I2C_PORT, addr, data, len, false);
    if (read_res < 0) {
        printf("❌ i2c_read_blocking failed with code %d\n", read_res);
        return BME280_E_COMM_FAIL;
    }

    printf("✅ I2C read OK, data:");
    for (uint32_t i = 0; i < len; ++i) {
        printf(" 0x%02X", data[i]);
    }
    printf("\n");

    return BME280_OK;
}


static int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr)
{
    printf("📤 [I2C WRITE] reg 0x%02X, len %lu\n", reg_addr, len);
    uint8_t addr = *(uint8_t *)intf_ptr;
    uint8_t buf[len + 1];
    buf[0] = reg_addr;
    memcpy(&buf[1], data, len);
    int res = i2c_write_blocking(I2C_PORT, addr, buf, len + 1, false);
    return res < 0 ? BME280_E_COMM_FAIL : BME280_OK;
}

static void user_delay_us(uint32_t period_us, void *intf_ptr)
{
    sleep_us(period_us);
}

int8_t bme280_init_default(void)
{
    printf("➡️ Start konfiguracji I2C\n");
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("➡️ Start konfiguracji struktury BME280\n");
    bme.intf_ptr = &dev_addr;
    bme.intf = BME280_I2C_INTF;
    bme.read = user_i2c_read;
    bme.write = user_i2c_write;
    bme.delay_us = user_delay_us;

    printf("➡️ bme280_init()...\n");
    int8_t res = bme280_init(&bme);
    if (res != BME280_OK) {
        printf("❌ bme280_init() ERROR: %d\n", res);
        return res;
    }

    printf("✅ bme280_init() OK\n");

    struct bme280_settings settings = {
        .osr_h = BME280_OVERSAMPLING_1X,
        .osr_p = BME280_OVERSAMPLING_16X,
        .osr_t = BME280_OVERSAMPLING_2X,
        .filter = BME280_FILTER_COEFF_16,
        .standby_time = BME280_STANDBY_TIME_62_5_MS
    };

    printf("➡️ Ustawiam ustawienia sensora...\n");
    res = bme280_set_sensor_settings(BME280_SEL_ALL_SETTINGS, &settings, &bme);
    if (res != BME280_OK) {
        printf("❌ bme280_set_sensor_settings() ERROR: %d\n", res);
        return res;
    }

    printf("➡️ Ustawiam tryb NORMAL...\n");
    res = bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &bme);
    if (res != BME280_OK) {
        printf("❌ bme280_set_sensor_mode() ERROR: %d\n", res);
        return res;
    }

    printf("✅ BME280 zainicjalizowany poprawnie\n");
    return BME280_OK;
}

int8_t bme280_read_data(struct bme280_data *comp_data)
{
    int8_t rslt = bme280_get_sensor_data(BME280_ALL, comp_data, &bme);
    return rslt;
}