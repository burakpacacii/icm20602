#ifndef MAIN_INC_ICM20602_H
#define MAIN_INC_ICM20602_H

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int32_t (*icmdev_write_ptr)(void *, uint8_t, const uint8_t *, uint16_t);
typedef int32_t (*icmdev_read_ptr)(void *, uint8_t, uint8_t *, uint16_t);

typedef struct {
    /** Component mandatory fields **/
    icmdev_write_ptr write_reg;
    icmdev_read_ptr read_reg;
    /** Customizable optional pointer **/
    void *handle;
} icmdev_ctx_t;

/***** Defines ICM20602 Registers *****/
#define ICM_REG_XG_OFFS_TC_H      0x04
#define ICM_REG_XG_OFFS_TC_L      0x05
#define ICM_REG_YG_OFFS_TC_H      0x07
#define ICM_REG_YG_OFFS_TC_L      0x08
#define ICM_REG_ZG_OFFS_TC_H      0x0A
#define ICM_REG_ZG_OFFS_TC_L      0x0B
#define ICM_REG_SELF_TEST_X_ACCEL 0x0D
#define ICM_REG_SELF_TEST_Y_ACCEL 0x0E
#define ICM_REG_SELF_TEST_Z_ACCEL 0x0F
// This is an undocumented register which if set incorrectly results in getting a 2.7m/s/s offset on the Y axis
// of the accelerometer
#define ICM_REG_UNDOC1             0x11
#define ICM_REG_UNDOC1_VALUE       0xC9 // Specific value for undocumented register
#define ICM_REG_XG_OFFS_USRH       0x13
#define ICM_REG_XG_OFFS_USRL       0x14
#define ICM_REG_YG_OFFS_USRH       0x15
#define ICM_REG_YG_OFFS_USRL       0x16
#define ICM_REG_ZG_OFFS_USRH       0x17
#define ICM_REG_ZG_OFFS_USRL       0x18
#define ICM_REG_SMPLRT_DIV         0x19
#define ICM_REG_CONFIG             0x1A
#define ICM_REG_GYRO_CONFIG        0x1B
#define ICM_REG_ACCEL_CONFIG       0x1C
#define ICM_REG_ACCEL_CONFIG_2     0x1D
#define ICM_REG_LP_MODE_CFG        0x1E
#define ICM_REG_ACCEL_WOM_X_THR    0x20
#define ICM_REG_ACCEL_WOM_Y_THR    0x21
#define ICM_REG_ACCEL_WOM_Z_THR    0x22
#define ICM_REG_FIFO_EN            0x23
#define ICM_REG_FSYNC_INT          0x36
#define ICM_REG_INT_PIN_CFG        0x37
#define ICM_REG_INT_ENABLE         0x38
#define ICM_REG_FIFO_WM_INT_STATUS 0x39
#define ICM_REG_INT_STATUS         0x3A
#define ICM_REG_ACCEL_XOUT_H       0x3B
#define ICM_REG_ACCEL_XOUT_L       0x3C
#define ICM_REG_ACCEL_YOUT_H       0x3D
#define ICM_REG_ACCEL_YOUT_L       0x3E
#define ICM_REG_ACCEL_ZOUT_H       0x3F
#define ICM_REG_ACCEL_ZOUT_L       0x40
#define ICM_REG_TEMP_OUT_H         0x41
#define ICM_REG_TEMP_OUT_L         0x42
#define ICM_REG_GYRO_XOUT_H        0x43
#define ICM_REG_GYRO_XOUT_L        0x44
#define ICM_REG_GYRO_YOUT_H        0x45
#define ICM_REG_GYRO_YOUT_L        0x46
#define ICM_REG_GYRO_ZOUT_H        0x47
#define ICM_REG_GYRO_ZOUT_L        0x48
#define ICM_REG_SELF_TEST_X_GYRO   0x50
#define ICM_REG_SELF_TEST_Y_GYRO   0x51
#define ICM_REG_SELF_TEST_Z_GYRO   0x52
#define ICM_REG_FIFO_WM_TH1        0x60
#define ICM_REG_FIFO_WM_TH2        0x61
#define ICM_REG_SIGNAL_PATH_RESET  0x68
#define ICM_REG_ACCEL_INTEL_CTRL   0x69
#define ICM_REG_USER_CTRL          0x6A
#define ICM_REG_PWR_MGMT_1         0x6B
#define ICM_REG_PWR_MGMT_2         0x6C
#define ICM_REG_I2C_IF             0x70
#define ICM_REG_FIFO_COUNTH        0x72
#define ICM_REG_FIFO_COUNTL        0x73
#define ICM_REG_FIFO_R_W           0x74
#define ICM_REG_WHO_AM_I           0x75
#define ICM_REG_XA_OFFSET_H        0x77
#define ICM_REG_XA_OFFSET_L        0x78
#define ICM_REG_YA_OFFSET_H        0x79
#define ICM_REG_YA_OFFSET_L        0x7A
#define ICM_REG_ZA_OFFSET_H        0x7B
#define ICM_REG_ZA_OFFSET_L        0x7C
#define ICM_WHO_AM_I               0x12

/***** Defines I2C *****/
#define ICM20602_I2C_DEV_ADDR 0x69

#define ICM_TEMP_SENSITIVITY 3268
#define ICM_ROOM_TEMP_OFFSET 25

#define ICM_ACCEL_SENSITIVITY_SHIFT_2G  14
#define ICM_ACCEL_SENSITIVITY_SHIFT_4G  13
#define ICM_ACCEL_SENSITIVITY_SHIFT_8G  12
#define ICM_ACCEL_SENSITIVITY_SHIFT_16G 11

#define ICM_GYRO_SENSITIVITY_250_DPS  1310
#define ICM_GYRO_SENSITIVITY_500_DPS  655
#define ICM_GYRO_SENSITIVITY_1000_DPS 328
#define ICM_GYRO_SENSITIVITY_2000_DPS 164

#define ICM_INT_PIN GPIO_NUM_5

typedef enum
{
    ICM_ACCEL_LPF_218HZ_RATE_1KHZ = 0,
    ICM_ACCEL_LPF_99HZ_RATE_1KHZ  = 2,
    ICM_ACCEL_LPF_44HZ_RATE_1KHZ  = 3,
    ICM_ACCEL_LPF_21HZ_RATE_1KHZ  = 4,
    ICM_ACCEL_LPF_10HZ_RATE_1KHZ  = 5,
    ICM_ACCEL_LPF_5HZ_RATE_1KHZ   = 6,
    ICM_ACCEL_LPF_420HZ_RATE_1KHZ = 7,
    ICM_ACCEL_LPF_BYPASS_1046HZ_RATE_4KHZ,
} icm_accel_dlpf_t;

typedef enum
{
    ICM_ACCEL_RANGE_2G  = 0,
    ICM_ACCEL_RANGE_4G  = 1,
    ICM_ACCEL_RANGE_8G  = 2,
    ICM_ACCEL_RANGE_16G = 3,
} icm_accel_g_range_t;

typedef enum
{
    ICM_GYRO_LPF_250HZ_RATE_8KHZ  = 0,
    ICM_GYRO_LPF_176HZ_RATE_1KHZ  = 1,
    ICM_GYRO_LPF_92HZ_RATE_1KHZ   = 2,
    ICM_GYRO_LPF_41HZ_RATE_1KHZ   = 3,
    ICM_GYRO_LPF_20HZ_RATE_1KHZ   = 4,
    ICM_GYRO_LPF_10HZ_RATE_1KHZ   = 5,
    ICM_GYRO_LPF_5HZ_RATE_1KHZ    = 6,
    ICM_GYRO_LPF_3281HZ_RATE_8KHZ = 7,
    ICM_GYRO_LPF_BYPASS_3281HZ_RATE_32KHZ,
    ICM_GYRO_LPF_BYPASS_8173HZ_RATE_32KHZ,
} icm_gyro_dlpf_t;

typedef enum
{
    ICM_GYRO_RANGE_250_DPS  = 0,
    ICM_GYRO_RANGE_500_DPS  = 1,
    ICM_GYRO_RANGE_1000_DPS = 2,
    ICM_GYRO_RANGE_2000_DPS = 3,
} icm_gyro_dps_t;

typedef union {
    uint8_t user_gyro_config;
    struct {
        uint8_t fchoice : 2;
        uint8_t         : 1;
        uint8_t fs_sel  : 2;
        uint8_t zg_set  : 1;
        uint8_t yg_set  : 1;
        uint8_t xg_set  : 1;
    } bits;

} icm_gyro_config_t;

typedef union {
    uint8_t user_accel_config;
    struct {
        uint8_t              : 3;
        uint8_t accel_fs_sel : 2;
        uint8_t za_st        : 1;
        uint8_t ya_st        : 1;
        uint8_t xa_st        : 1;
    } bits;

} icm_accel_config_t;

typedef union {
    uint8_t user_accel_config2;
    struct {
        uint8_t a_dlpf_cfg      : 3;
        uint8_t accel_fchoice_b : 1;
        uint8_t dec2_cfg        : 2;
        uint8_t                 : 2;
    } bits;

} icm_accel_config2_t;

typedef union {
    uint8_t user_config;
    struct {
        uint8_t dlpf_cfg       : 3;
        uint8_t ext_sync_set   : 3;
        uint8_t fifo_mode      : 1;
        uint8_t default_config : 1;
    } bits;

} icm_config_t;

typedef union {
    uint8_t user_gyro_low_power_mode_config;
    struct {
        uint8_t            : 4;
        uint8_t g_avgcfg   : 3;
        uint8_t gyro_cycle : 1;
    } bits;

} icm_gyro_low_power_mode_config_t;

typedef union {
    uint8_t user_fifo_enable;
    struct {
        uint8_t               : 3;
        uint8_t accel_fifo_en : 1;
        uint8_t gyro_fifo_en  : 1;
        uint8_t               : 3;
    } bits;

} icm_fifo_enable_t;

typedef union {
    uint8_t user_int_enable;
    struct {
        uint8_t data_rdy_int_en : 1;
        uint8_t                 : 1;
        uint8_t gdrive_int_en   : 1;
        uint8_t fsync_int_en    : 1;
        uint8_t fifo_oflow_en   : 1;
        uint8_t wom_z_int_en    : 1;
        uint8_t wom_y_int_en    : 1;
        uint8_t wom_x_int_en    : 1;
    } bits;

} icm_int_enable_t;

typedef union {
    uint8_t user_int_pin_config;
    struct {
        uint8_t                   : 2;
        uint8_t fsync_int_mode_en : 1;
        uint8_t fsync_int_level   : 1;
        uint8_t int_rd_clear      : 1;
        uint8_t latch_int_en      : 1;
        uint8_t int_open          : 1;
        uint8_t int_level         : 1;
    } bits;

} icm_int_pin_config_t;

typedef union {
    uint8_t user_fifo_wm_int_status;
    struct {
        uint8_t             : 6;
        uint8_t fifo_wm_int : 1;
        uint8_t             : 1;
    } bits;

} icm_fifo_wm_int_status_t;

typedef union {
    uint8_t user_int_status;
    struct {
        uint8_t data_rdy_int   : 1;
        uint8_t                : 1;
        uint8_t gdrive_int     : 1;
        uint8_t                : 1;
        uint8_t fifo_oflow_int : 1;
        uint8_t wom_z_int      : 1;
        uint8_t wom_y_int      : 1;
        uint8_t wom_x_int      : 1;
    } bits;

} icm_int_status_t;

typedef union {
    uint8_t user_signal_path_reset;
    struct {
        uint8_t temp_rst  : 1;
        uint8_t accel_rst : 1;
        uint8_t           : 6;
    } bits;

} icm_signal_path_reset_t;

typedef union {
    uint8_t user_accel_intel_ctrl;
    struct {
        uint8_t wom_th_mode      : 1;
        uint8_t output_limit     : 1;
        uint8_t                  : 4;
        uint8_t accel_intel_mode : 1;
        uint8_t accel_intel_en   : 1;
    } bits;

} icm_accel_intel_ctrl_t;

typedef union {
    uint8_t user_ctrl;
    struct {
        uint8_t sig_cond_rst : 1;
        uint8_t              : 1;
        uint8_t fifo_rst     : 1;
        uint8_t              : 3;
        uint8_t fifo_en      : 1;
        uint8_t              : 1;
    } bits;

} icm_user_ctrl_t;

typedef union {
    uint8_t user_power_managment1;
    struct {
        uint8_t clksel       : 3;
        uint8_t temp_dis     : 1;
        uint8_t gyro_standby : 1;
        uint8_t cycle        : 1;
        uint8_t sleep        : 1;
        uint8_t device_reset : 1;
    } bits;

} icm_power_managment1_t;

typedef union {
    uint8_t user_power_managment2;
    struct {
        uint8_t stby_zg : 1;
        uint8_t stby_yg : 1;
        uint8_t stby_xg : 1;
        uint8_t stby_za : 1;
        uint8_t stby_ya : 1;
        uint8_t stby_xa : 1;
        uint8_t         : 2;
    } bits;

} icm_power_managment2_t;

typedef union {
    uint16_t user_accel_offset;
    struct {
        uint16_t        : 1;
        uint16_t offset : 14;
        uint16_t sign   : 1;
    } bits;
} accel_offset_t;

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int8_t temp;
} icm_data_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} icm_offset_t;

typedef struct {
    uint8_t fifoRowLen;
    uint8_t accel_sensitivity;
    uint16_t gyro_sensitivity;
    icm_offset_t accel_offset;
    icm_offset_t gyro_offset;
} icm_dev_t;

void icmReset(icmdev_ctx_t *ctx);
void icmSetClock(icmdev_ctx_t *ctx, uint8_t clock_source);
void icmSetSampleRate(icmdev_ctx_t *ctx, uint16_t sample_ratehz);
void icmSetSleep(icmdev_ctx_t *ctx, bool enable);
void icmSetFIFO(icmdev_ctx_t *ctx, bool acc_enable, bool gyro_enable);
void icmSetFIFOInt(icmdev_ctx_t *ctx, bool enable);
void icmSetAccelOffsetAxis(icmdev_ctx_t *ctx, uint16_t x_offset, uint16_t y_offset, uint16_t z_offset);
void icmGetAccelOffsetAxis(icmdev_ctx_t *ctx, icm_offset_t *accel_offset);
void icmSetAccelAxis(icmdev_ctx_t *ctx, bool enable_x, bool enable_y, bool enable_z);
void icmSetAccelLPF(icmdev_ctx_t *ctx, icm_accel_dlpf_t accel_dlpf);
void icmSetAccelGRange(icmdev_ctx_t *ctx, icm_accel_g_range_t accel_g_range);
void icmSetAccelWoMThresholdAxis(icmdev_ctx_t *ctx, uint8_t x_wom_th, uint8_t y_wom_th, uint8_t z_wom_th);
void icmSetWaterMarkThreshold(icmdev_ctx_t *ctx, uint16_t watermark_th);
void icmSetGyroOffsetAxis(icmdev_ctx_t *ctx, uint16_t x_offset, uint16_t y_offset, uint16_t z_offset);
void icmGetGyroOffsetAxis(icmdev_ctx_t *ctx, icm_offset_t *gyro_offset);
void icmSetGyroAxis(icmdev_ctx_t *ctx, bool enable_x, bool enable_y, bool enable_z);
void icmSetGyroLPF(icmdev_ctx_t *ctx, icm_gyro_dlpf_t gyro_dlpf);
void icmSetGyroDPS(icmdev_ctx_t *ctx, icm_gyro_dps_t gyro_dps);
void icmGetGyroOffsetAxis(icmdev_ctx_t *ctx, icm_offset_t *gyro_offset);
void icmGetAccelDataWithTemp(icmdev_ctx_t *ctx, icm_data_t *p_accel);
void icmGetGyroData(icmdev_ctx_t *ctx, icm_data_t *p_gyro);
void icmGetAccelGyroData(icmdev_ctx_t *ctx, icm_data_t *p_accel, icm_data_t *p_gyro);
void icmGetTempData(icmdev_ctx_t *ctx, int16_t *p_TempData);
void icmGetFifoAccelData(icmdev_ctx_t *ctx);
void icmGetFifoGyroData(icmdev_ctx_t *ctx);
void icmGetFifoAccelGyroData(icmdev_ctx_t *ctx);

#endif /* MAIN_INC_ICM20602_H */