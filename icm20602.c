#include "icm20602.h"

static icm_dev_t icmConfig = {0};

/**
 * @brief IMU reset.
 *
 */
void icmReset(icmdev_ctx_t *ctx)
{
    icm_power_managment1_t power_managment1 = {0};
    power_managment1.bits.device_reset      = true;
    power_managment1.bits.temp_dis          = true;
    ctx->write_reg(NULL, ICM_REG_PWR_MGMT_1, &power_managment1.user_power_managment1, 1);
}

/**
 * @brief Set the clock source to IMU.
 *
 * @param clock_source 0,6: Internal 20 MHz oscillator
 *                     1,2,3,4,5: Auto selects the best available clock source � PLL if ready, else use the Internal
 * oscillator 7: Stops the clock and keeps timing generator in reset
 */
void icmSetClock(icmdev_ctx_t *ctx, uint8_t clock_source)
{
    icm_power_managment1_t power_managment1 = {0};
    ctx->read_reg(NULL, ICM_REG_PWR_MGMT_1, &power_managment1.user_power_managment1, 1);
    power_managment1.bits.clksel = clock_source;
    ctx->write_reg(NULL, ICM_REG_PWR_MGMT_1, &power_managment1.user_power_managment1, 1);
}

/**
 * @brief Set the sample rate divider to IMU.
 *
 * @param sample_rate_hz Sample rate range should be 4 - 1000.
 *                       1 = 1 Hz, 1000 = 1Khz.
 */
void icmSetSampleRate(icmdev_ctx_t *ctx, uint16_t sample_rate_hz)
{
    uint8_t val = 0;
    if (sample_rate_hz <= 4)
    {
        sample_rate_hz = 4;
    }
    if (sample_rate_hz >= 1000)
    {
        sample_rate_hz = 1000;
    }
    val = (1000 / sample_rate_hz) - 1;
    ctx->write_reg(NULL, ICM_REG_SMPLRT_DIV, &val, 1);
}

/**
 * @brief Set FIFO interrupt enable or disable.
 *
 * @param enable 0: Reset
 *               1: Set
 */
void icmSetFIFOInt(icmdev_ctx_t *ctx, bool enable)
{
    icm_int_pin_config_t int_pin_config = {0};
    int_pin_config.bits.latch_int_en    = true;
    ctx->write_reg(NULL, ICM_REG_INT_PIN_CFG, &int_pin_config.user_int_pin_config, 1);

    icm_int_enable_t int_enable   = {0};
    int_enable.bits.fifo_oflow_en = true;
    ctx->write_reg(NULL, ICM_REG_INT_ENABLE, &int_enable.user_int_enable, 1);
}

/**
 * @brief  Set Water-mark threshold level. This function adjusts the FIFO boundary then can be getting data
 *         from water-mark interrupt when which limit set.
 *
 * @note   Before set to water-mark threshold ensure that configuration register should be 0.
 *         The threshold level should be coefficient of which part has enabled and which axis has enabled.
 *         Example: If all axis of accelerometer is enable that means 6 bytes and temperature is 2 byte sum of them is 8
 * bytes. Threshold value should be coefficient of 8.
 *
 * @param wm_threshold Accel or Gyro enable -> Threshold should be max 126(row).
 *                     Accel and gyro enable -> Threshold should be max 72(row).
 *                     0: Disable.
 *                     Default value is 0.
 */
void icmSetWaterMarkThreshold(icmdev_ctx_t *ctx, uint16_t wm_threshold)
{
    icm_config_t config = {0};
    config.user_config  = 0;
    if (icmConfig.fifoRowLen == 14)
    {
        if (wm_threshold >= 72)
        {
            wm_threshold = 72;
        }
        wm_threshold *= icmConfig.fifoRowLen;
    }

    if (icmConfig.fifoRowLen == 8)
    {
        if (wm_threshold >= 126)
        {
            wm_threshold = 126;
        }
        wm_threshold *= icmConfig.fifoRowLen;
    }

    ctx->write_reg(NULL, ICM_REG_CONFIG, &config.user_config, 1);
    uint8_t vmThreshold[2] = {0};
    vmThreshold[0]         = (uint8_t)(wm_threshold >> 8);
    vmThreshold[1]         = (uint8_t)(wm_threshold & 0xFF);
    ctx->write_reg(NULL, ICM_REG_FIFO_WM_TH1, vmThreshold, 2);
}

// TODO
/**
 * @brief Use Accelerometer with wake on motion mode. Set the threshold value then check WoM interrupt
 *        when accelerometer data is greater than WoM threshold data.
 *
 * @note WoM threshold should be set inside of accelerometer g boundaries.
 *
 * @param x_wom_th
 *        0: Disable
 * @param y_wom_th
 *        0: Disable
 * @param z_wom_th
 *        0: Disable
 */
void icmSetAccelWoMThresholdAxis(icmdev_ctx_t *ctx, uint8_t x_wom_th, uint8_t y_wom_th, uint8_t z_wom_th)
{
    icm_accel_intel_ctrl_t accel_intel_ctrl = {0};
    icm_int_enable_t int_enable             = {0};

    ctx->read_reg(NULL, ICM_REG_INT_ENABLE, &int_enable.user_int_enable, 1);
    if (x_wom_th != 0)
    {
        int_enable.bits.wom_x_int_en = true;
        ctx->write_reg(NULL, ICM_REG_ACCEL_WOM_X_THR, &x_wom_th, 1);
    }
    else
    {
        int_enable.bits.wom_x_int_en = false;
    }
    if (y_wom_th != 0)
    {
        int_enable.bits.wom_y_int_en = true;
        ctx->write_reg(NULL, ICM_REG_ACCEL_WOM_Y_THR, &y_wom_th, 1);
    }
    else
    {
        int_enable.bits.wom_y_int_en = false;
    }
    if (z_wom_th != 0)
    {
        int_enable.bits.wom_z_int_en = true;
        ctx->write_reg(NULL, ICM_REG_ACCEL_WOM_Z_THR, &z_wom_th, 1);
    }
    else
    {
        int_enable.bits.wom_z_int_en = false;
    }
    ctx->write_reg(NULL, ICM_REG_INT_ENABLE, &int_enable.user_int_enable, 1);
    if (x_wom_th || y_wom_th || z_wom_th)
    {
        accel_intel_ctrl.bits.accel_intel_en = true;
    }

    //    accel_intel_ctrl.bits.accel_intel_mode = true;  //  1 - Compare the current sample with the previous sample
    //    accel_intel_ctrl.bits.wom_th_mode = true;       //  1 - WoM int AND mode    0 - WoM int OR mode
    ctx->write_reg(NULL, ICM_REG_ACCEL_INTEL_CTRL, &accel_intel_ctrl.user_accel_intel_ctrl, 1);
}

/**
 * @brief Set accelerometer low pass filter.
 *
 * @param accel_dlpf select the rate of LPF @icm_accel_dlpf_t
 */
void icmSetAccelLPF(icmdev_ctx_t *ctx, icm_accel_dlpf_t accel_dlpf)
{
    icm_accel_config2_t accel_config2 = {0};

    if (accel_dlpf == ICM_ACCEL_LPF_BYPASS_1046HZ_RATE_4KHZ)
    {
        ctx->read_reg(NULL, ICM_REG_ACCEL_CONFIG_2, &accel_config2.user_accel_config2, 1);
        accel_config2.bits.accel_fchoice_b = true;
        ctx->write_reg(NULL, ICM_REG_ACCEL_CONFIG_2, &accel_config2.user_accel_config2, 1);
    }
    else
    {
        ctx->read_reg(NULL, ICM_REG_ACCEL_CONFIG_2, &accel_config2.user_accel_config2, 1);
        accel_config2.bits.a_dlpf_cfg      = accel_dlpf;
        accel_config2.bits.accel_fchoice_b = false;
        ctx->write_reg(NULL, ICM_REG_ACCEL_CONFIG_2, &accel_config2.user_accel_config2, 1);
    }
}

/**
 * @brief Set accelerometer G range.
 *
 * @param accel_g_range select the rage of G value @icm_accel_g_range_t
 */
void icmSetAccelGRange(icmdev_ctx_t *ctx, icm_accel_g_range_t accel_g_range)
{
    icm_accel_config_t accel_config = {0};
    ctx->read_reg(NULL, ICM_REG_ACCEL_CONFIG, &accel_config.user_accel_config, 1);
    accel_config.bits.accel_fs_sel = accel_g_range;
    ctx->write_reg(NULL, ICM_REG_ACCEL_CONFIG, &accel_config.user_accel_config, 1);

    switch (accel_g_range)
    {
    case (ICM_ACCEL_RANGE_2G):
        icmConfig.accel_sensitivity = ICM_ACCEL_SENSITIVITY_SHIFT_2G;
        break;
    case (ICM_ACCEL_RANGE_4G):
        icmConfig.accel_sensitivity = ICM_ACCEL_SENSITIVITY_SHIFT_4G;
        break;
    case (ICM_ACCEL_RANGE_8G):
        icmConfig.accel_sensitivity = ICM_ACCEL_SENSITIVITY_SHIFT_8G;
        break;
    case (ICM_ACCEL_RANGE_16G):
        icmConfig.accel_sensitivity = ICM_ACCEL_SENSITIVITY_SHIFT_16G;
        break;
    }
}

// TODO
/**
 * @brief Set accelerometer axis to offset value.
 *
 * @param x_offset
 * @param y_offset
 * @param z_offset
 */
void icmSetAccelOffsetAxis(icmdev_ctx_t *ctx, uint16_t x_offset, uint16_t y_offset, uint16_t z_offset)
{
    uint8_t offset[2] = {0};

    offset[0] = (uint8_t)(x_offset >> 8);
    offset[1] = (uint8_t)(x_offset & 0xFF);
    ctx->write_reg(NULL, ICM_REG_XA_OFFSET_H, offset, 2);
    offset[0] = (uint8_t)(y_offset >> 8);
    offset[1] = (uint8_t)(y_offset & 0xFF);
    ctx->write_reg(NULL, ICM_REG_YA_OFFSET_H, offset, 2);
    offset[0] = (uint8_t)(z_offset >> 8);
    offset[1] = (uint8_t)(z_offset & 0xFF);
    ctx->write_reg(NULL, ICM_REG_ZA_OFFSET_H, offset, 2);
}

/**
 * @brief Get accelerometer axis to offset value.
 *
 * @param accel_offset Select which axis of offset get @icm_offset_t
 */
void icmGetAccelOffsetAxis(icmdev_ctx_t *ctx, icm_offset_t *accel_offset)
{
    uint8_t offset_val[6] = {0};

    ctx->read_reg(NULL, ICM_REG_XA_OFFSET_H, offset_val, 6);
    accel_offset->x = ((uint16_t)offset_val[0] << 8) | offset_val[1];
    accel_offset->y = ((uint16_t)offset_val[2] << 8) | offset_val[3];
    accel_offset->z = ((uint16_t)offset_val[4] << 8) | offset_val[5];
}

/**
 * @brief Set accelerometer axis enable or disable.
 *
 * @note Default settings of all axis are enable.
 *
 * @param accel_x 0: Reset
 *                1: Set
 * @param accel_y 0: Reset
 *                1: Set
 * @param accel_z 0: Reset
 *                1: Set
 */
void icmSetAccelAxis(icmdev_ctx_t *ctx, bool accel_x, bool accel_y, bool accel_z)
{
    icm_power_managment2_t power_managment2 = {0};
    ctx->read_reg(NULL, ICM_REG_PWR_MGMT_2, &power_managment2.user_power_managment2, 1);

    power_managment2.bits.stby_za = !accel_x;
    power_managment2.bits.stby_ya = !accel_y;
    power_managment2.bits.stby_xa = !accel_z;

    if ((accel_x == true) || (accel_y == true) || (accel_z == true))
    {
        if (icmConfig.fifoRowLen == 0)
        {
            icmConfig.fifoRowLen = 8;
        }
        else
        {
            icmConfig.fifoRowLen = 14;
        }
    }
    ctx->write_reg(NULL, ICM_REG_PWR_MGMT_2, &power_managment2.user_power_managment2, 1);
}

/**
 * @brief Set accelerometer and gyroscope FIFO enable or disable.
 *
 * @param acc_enable 0: Reset
 *                   1: Set
 * @param gyro_enable 0: Reset
 *                    1: Set
 */
void icmSetFIFO(icmdev_ctx_t *ctx, bool acc_enable, bool gyro_enable)
{
    icm_user_ctrl_t user_ctrl     = {0};
    icm_fifo_enable_t fifo_enable = {0};

    ctx->read_reg(NULL, ICM_REG_FIFO_EN, &fifo_enable.user_fifo_enable, 1);
    fifo_enable.bits.accel_fifo_en = acc_enable;
    fifo_enable.bits.gyro_fifo_en  = gyro_enable;
    ctx->write_reg(NULL, ICM_REG_FIFO_EN, &fifo_enable.user_fifo_enable, 1);

    ctx->read_reg(NULL, ICM_REG_USER_CTRL, &user_ctrl.user_ctrl, 1);
    user_ctrl.bits.fifo_en = (acc_enable | gyro_enable);
    ctx->write_reg(NULL, ICM_REG_USER_CTRL, &user_ctrl.user_ctrl, 1);
}

/**
 * @brief Set gyroscope axis enable or disable.
 *
 * @note Default settings of all axis are enable.
 *
 * @param gyro_x 0: Reset
 *               1: Set
 * @param gyro_y 0: Reset
 *               1: Set
 * @param gyro_z 0: Reset
 *               1: Set
 */
void icmSetGyroAxis(icmdev_ctx_t *ctx, bool gyro_x, bool gyro_y, bool gyro_z)
{
    icm_power_managment2_t power_managment2 = {0};
    ctx->read_reg(NULL, ICM_REG_PWR_MGMT_2, &power_managment2.user_power_managment2, 1);

    power_managment2.bits.stby_zg = !gyro_z;
    power_managment2.bits.stby_yg = !gyro_y;
    power_managment2.bits.stby_xg = !gyro_x;

    if ((gyro_x == true) || (gyro_y == true) || (gyro_z == true))
    {
        if (icmConfig.fifoRowLen == 0)
        {
            icmConfig.fifoRowLen = 8;
        }
        else
        {
            icmConfig.fifoRowLen = 14;
        }
    }
    ctx->write_reg(NULL, ICM_REG_PWR_MGMT_2, &power_managment2.user_power_managment2, 1);
}

/**
 * @brief Set gyroscope low pass filter.
 *
 * @param gyro_dlpf select the rate of LPF @icm_gyro_dlpf_t
 */
void icmSetGyroLPF(icmdev_ctx_t *ctx, icm_gyro_dlpf_t gyro_dlpf)
{
    icm_config_t config           = {0};
    icm_gyro_config_t gyro_config = {0};

    if (gyro_dlpf == ICM_GYRO_LPF_BYPASS_3281HZ_RATE_32KHZ)
    {
        ctx->read_reg(NULL, ICM_REG_CONFIG, &config.user_config, 1);
        config.bits.dlpf_cfg = 0;
        ctx->write_reg(NULL, ICM_REG_CONFIG, &config.user_config, 1);

        ctx->read_reg(NULL, ICM_REG_GYRO_CONFIG, &gyro_config.user_gyro_config, 1);
        gyro_config.bits.fchoice = 2;
        ctx->write_reg(NULL, ICM_REG_GYRO_CONFIG, &gyro_config.user_gyro_config, 1);
    }
    else if (gyro_dlpf == ICM_GYRO_LPF_BYPASS_8173HZ_RATE_32KHZ)
    {
        ctx->read_reg(NULL, ICM_REG_CONFIG, &config.user_config, 1);
        config.bits.dlpf_cfg = 0;
        ctx->write_reg(NULL, ICM_REG_CONFIG, &config.user_config, 1);

        ctx->read_reg(NULL, ICM_REG_GYRO_CONFIG, &gyro_config.user_gyro_config, 1);
        gyro_config.bits.fchoice = 1;
        ctx->write_reg(NULL, ICM_REG_GYRO_CONFIG, &gyro_config.user_gyro_config, 1);
    }
    else
    {
        ctx->read_reg(NULL, ICM_REG_CONFIG, &config.user_config, 1);
        config.bits.dlpf_cfg = gyro_dlpf;
        ctx->write_reg(NULL, ICM_REG_CONFIG, &config.user_config, 1);

        ctx->read_reg(NULL, ICM_REG_GYRO_CONFIG, &gyro_config.user_gyro_config, 1);
        gyro_config.bits.fchoice = 0;
        ctx->write_reg(NULL, ICM_REG_GYRO_CONFIG, &gyro_config.user_gyro_config, 1);
    }
}

/**
 * @brief Set gyroscope degree per seconds level.
 *
 * @param gyro_dps Select the rate of DPS @icm_gyro_dps_t
 */
void icmSetGyroDPS(icmdev_ctx_t *ctx, icm_gyro_dps_t gyro_dps)
{
    icm_gyro_config_t gyro_config = {0};
    ctx->read_reg(NULL, ICM_REG_GYRO_CONFIG, &gyro_config.user_gyro_config, 1);
    gyro_config.bits.fs_sel = gyro_dps;
    ctx->write_reg(NULL, ICM_REG_GYRO_CONFIG, &gyro_config.user_gyro_config, 1);

    switch (gyro_dps)
    {
    case (ICM_GYRO_RANGE_250_DPS):
        icmConfig.gyro_sensitivity = ICM_GYRO_SENSITIVITY_250_DPS;
        break;
    case (ICM_GYRO_RANGE_500_DPS):
        icmConfig.gyro_sensitivity = ICM_GYRO_SENSITIVITY_500_DPS;
        break;
    case (ICM_GYRO_RANGE_1000_DPS):
        icmConfig.gyro_sensitivity = ICM_GYRO_SENSITIVITY_1000_DPS;
        break;
    case (ICM_GYRO_RANGE_2000_DPS):
        icmConfig.gyro_sensitivity = ICM_GYRO_SENSITIVITY_2000_DPS;
        break;
    }
}

// TODO
/**
 * @brief
 *
 * @param x_offset
 * @param y_offset
 * @param z_offset
 */
void icmSetGyroOffsetAxis(icmdev_ctx_t *ctx, uint16_t x_offset, uint16_t y_offset, uint16_t z_offset)
{
    uint8_t offset[2] = {0};

    offset[0] = (uint8_t)(x_offset >> 8);
    offset[1] = (uint8_t)(x_offset & 0xFF);
    ctx->write_reg(NULL, ICM_REG_XG_OFFS_USRH, offset, 2);
    offset[0] = (uint8_t)(y_offset >> 8);
    offset[1] = (uint8_t)(y_offset & 0xFF);
    ctx->write_reg(NULL, ICM_REG_YG_OFFS_USRH, offset, 2);
    offset[0] = (uint8_t)(z_offset >> 8);
    offset[1] = (uint8_t)(z_offset & 0xFF);
    ctx->write_reg(NULL, ICM_REG_ZG_OFFS_USRH, offset, 2);
}

/**
 * @brief Get gyroscope axis to offset value.
 *
 * @param gyro_offset Select which axis of offset get @icm_offset_t
 */
void icmGetGyroOffsetAxis(icmdev_ctx_t *ctx, icm_offset_t *gyro_offset)
{
    uint8_t offset_val[6] = {0};
    ctx->read_reg(NULL, ICM_REG_XG_OFFS_USRH, offset_val, 6);

    gyro_offset->x = ((uint16_t)offset_val[0] << 8) | offset_val[1];
    gyro_offset->y = ((uint16_t)offset_val[2] << 8) | offset_val[3];
    gyro_offset->z = ((uint16_t)offset_val[4] << 8) | offset_val[5];
}

/**
 * @brief Set IMU sleep mode.
 *
 * @param enable_disable 0: Reset
 *                       1: Set
 */
void icmSetSleep(icmdev_ctx_t *ctx, bool enable)
{
    icm_power_managment1_t power_managment1 = {0};
    ctx->read_reg(NULL, ICM_REG_PWR_MGMT_1, &power_managment1.user_power_managment1, 1);
    power_managment1.bits.sleep = enable;
    ctx->write_reg(NULL, ICM_REG_PWR_MGMT_1, &power_managment1.user_power_managment1, 1);
}

/**
 * @brief Get accelerometer data in type of milli-g
 *
 * @param p_accel identify input for typedef then get data from struct.
 */
void icmGetAccelDataWithTemp(icmdev_ctx_t *ctx, icm_data_t *p_accel)
{
    uint8_t rawDataBuffer[8];
    ctx->read_reg(NULL, ICM_REG_ACCEL_XOUT_H, rawDataBuffer, 8);
    p_accel->accel_x = ((int16_t)(rawDataBuffer[0] << 8 | rawDataBuffer[1]) * 1000) >> icmConfig.accel_sensitivity;
    p_accel->accel_y = ((int16_t)(rawDataBuffer[2] << 8 | rawDataBuffer[3]) * 1000) >> icmConfig.accel_sensitivity;
    p_accel->accel_z = ((int16_t)(rawDataBuffer[4] << 8 | rawDataBuffer[5]) * 1000) >> icmConfig.accel_sensitivity;
    p_accel->temp
        = (((int16_t)(rawDataBuffer[6] << 8 | rawDataBuffer[7]) * 10) / ICM_TEMP_SENSITIVITY) + ICM_ROOM_TEMP_OFFSET;
}

/**
 * @brief Get gyroscope data
 *
 * @param p_gyro identify input for typedef type then get data from struct.
 */
void icmGetGyroData(icmdev_ctx_t *ctx, icm_data_t *p_gyro)
{
    uint8_t rawDataBuffer[6];
    ctx->read_reg(NULL, ICM_REG_GYRO_XOUT_H, rawDataBuffer, 6);
    p_gyro->gyro_x = ((int16_t)(rawDataBuffer[0] << 8 | rawDataBuffer[1]) * 10) / icmConfig.gyro_sensitivity;
    p_gyro->gyro_y = ((int16_t)(rawDataBuffer[2] << 8 | rawDataBuffer[3]) * 10) / icmConfig.gyro_sensitivity;
    p_gyro->gyro_z = ((int16_t)(rawDataBuffer[4] << 8 | rawDataBuffer[5]) * 10) / icmConfig.gyro_sensitivity;
}

/**
 * @brief Get accelerometer and gyroscope data.
 *
 * @param p_accel
 * @param p_gyro
 */
void icmGetAccelGyroData(icmdev_ctx_t *ctx, icm_data_t *p_accel, icm_data_t *p_gyro)
{
    uint8_t rawDataBuffer[14];
    ctx->read_reg(NULL, ICM_REG_ACCEL_XOUT_H, rawDataBuffer, 14);
    p_accel->accel_x = ((int16_t)(rawDataBuffer[0] << 8 | rawDataBuffer[1]) * 1000) >> icmConfig.accel_sensitivity;
    p_accel->accel_y = ((int16_t)(rawDataBuffer[2] << 8 | rawDataBuffer[3]) * 1000) >> icmConfig.accel_sensitivity;
    p_accel->accel_z = ((int16_t)(rawDataBuffer[4] << 8 | rawDataBuffer[5]) * 1000) >> icmConfig.accel_sensitivity;
    p_gyro->gyro_x   = ((int16_t)(rawDataBuffer[8] << 8 | rawDataBuffer[9]) * 10) / icmConfig.gyro_sensitivity;
    p_gyro->gyro_y   = ((int16_t)(rawDataBuffer[10] << 8 | rawDataBuffer[11]) * 10) / icmConfig.gyro_sensitivity;
    p_gyro->gyro_z   = ((int16_t)(rawDataBuffer[12] << 8 | rawDataBuffer[13]) * 10) / icmConfig.gyro_sensitivity;
    p_gyro->temp
        = (((int16_t)(rawDataBuffer[6] << 8 | rawDataBuffer[7]) * 10) / ICM_TEMP_SENSITIVITY) + ICM_ROOM_TEMP_OFFSET;
}

/**
 * @brief Get accelerometer and temperature data from FIFO.
 *
 * @note The data is received sequentially from FIFO as accel-x, accel-y, accel-z, temperature.
 *       Even if the temperature is disabled, FIFO always keeps giving you the temperature data.
 */
void icmGetFifoAccelData(icmdev_ctx_t *ctx)
{
    uint8_t fifoCountBuff[2] = {0};
    ctx->read_reg(NULL, ICM_REG_FIFO_COUNTH, fifoCountBuff, 2);
}

/**
 * @brief Get gyroscope and temperature data from FIFO.
 *
 * @note The data is received sequentially from FIFO as temperature, gyro-x, gyro-y, gyro-z.
 *       Even if the temperature is disabled, FIFO always keeps giving you the temperature data.
 */
void icmGetFifoGyroData(icmdev_ctx_t *ctx)
{
    uint8_t fifoCountBuff[2] = {0};
    ctx->read_reg(NULL, ICM_REG_FIFO_COUNTH, fifoCountBuff, 2);
}

void icmGetFifoAccelGyroData(icmdev_ctx_t *ctx)
{
    uint8_t fifoCountBuff[2] = {0};
    ctx->read_reg(NULL, ICM_REG_FIFO_COUNTH, fifoCountBuff, 2);
}

// EOF