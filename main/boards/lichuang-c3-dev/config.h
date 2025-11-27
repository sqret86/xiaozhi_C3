#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_13
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_8
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_12
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_10
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_7
/*为了优化布线和处理不能正常修改GPIO_NUM_11状态，2025年7月31日修改了引脚*/

#define AUDIO_CODEC_USE_PCA9557
#define AUDIO_CODEC_PA_PIN       GPIO_NUM_NC
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_0
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_1
#define AUDIO_CODEC_ES8311_ADDR  ES8311_CODEC_DEFAULT_ADDR
#define AUDIO_CODEC_ES7210_ADDR  0x82


#define BOOT_BUTTON_GPIO        GPIO_NUM_9
#define VOLUME_UP_BUTTON_GPIO   GPIO_NUM_NC
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_NC

/*7月28日版本使用该引脚，该版本的屏幕排线画反了，已让小林画结构，组装后给合作商调试打模型*/
#define DISPLAY_SPI_SCK_PIN     GPIO_NUM_3
#define DISPLAY_SPI_MOSI_PIN    GPIO_NUM_4
#define DISPLAY_DC_PIN          GPIO_NUM_6
#define DISPLAY_SPI_CS_PIN      GPIO_NUM_5


/*8月6日版本使用该引脚，已PCB打样，后续需焊接器件，可共用小林设计的结构*/
/*#define DISPLAY_SPI_SCK_PIN     GPIO_NUM_4
#define DISPLAY_SPI_MOSI_PIN    GPIO_NUM_3
#define DISPLAY_DC_PIN          GPIO_NUM_5
#define DISPLAY_SPI_CS_PIN      GPIO_NUM_6
*/
// ============================================================================
// 屏幕旋转配置 - 只需修改 DISPLAY_ROTATION 即可切换屏幕方向
// ============================================================================
// 可选值：
//   0   - 竖屏，正常方向
//   90  - 横屏，顺时针旋转90度
//   180 - 竖屏，倒置180度
//   270 - 横屏，顺时针旋转270度
// ============================================================================
#define DISPLAY_ROTATION 180 //角度是按极坐标的方向

// 1.69寸屏幕物理分辨率
#define DISPLAY_PHYSICAL_WIDTH  240
#define DISPLAY_PHYSICAL_HEIGHT 280
#define DISPLAY_PHYSICAL_OFFSET 20

// 根据旋转角度自动配置显示参数
#if DISPLAY_ROTATION == 0
    // 竖屏 0度
    #define DISPLAY_WIDTH   DISPLAY_PHYSICAL_WIDTH
    #define DISPLAY_HEIGHT  DISPLAY_PHYSICAL_HEIGHT
    #define DISPLAY_MIRROR_X false
    #define DISPLAY_MIRROR_Y false
    #define DISPLAY_SWAP_XY false
    #define DISPLAY_OFFSET_X  0
    #define DISPLAY_OFFSET_Y  DISPLAY_PHYSICAL_OFFSET

#elif DISPLAY_ROTATION == 90
    // 横屏 90度
    #define DISPLAY_WIDTH   DISPLAY_PHYSICAL_HEIGHT
    #define DISPLAY_HEIGHT  DISPLAY_PHYSICAL_WIDTH
    #define DISPLAY_MIRROR_X false
    #define DISPLAY_MIRROR_Y true
    #define DISPLAY_SWAP_XY true
    #define DISPLAY_OFFSET_X  DISPLAY_PHYSICAL_OFFSET
    #define DISPLAY_OFFSET_Y  0

#elif DISPLAY_ROTATION == 180
    // 竖屏 180度
    #define DISPLAY_WIDTH   DISPLAY_PHYSICAL_WIDTH
    #define DISPLAY_HEIGHT  DISPLAY_PHYSICAL_HEIGHT
    #define DISPLAY_MIRROR_X true
    #define DISPLAY_MIRROR_Y true
    #define DISPLAY_SWAP_XY false
    #define DISPLAY_OFFSET_X  0
    #define DISPLAY_OFFSET_Y  DISPLAY_PHYSICAL_OFFSET

#elif DISPLAY_ROTATION == 270
    // 横屏 270度
    #define DISPLAY_WIDTH   DISPLAY_PHYSICAL_HEIGHT
    #define DISPLAY_HEIGHT  DISPLAY_PHYSICAL_WIDTH
    #define DISPLAY_MIRROR_X true
    #define DISPLAY_MIRROR_Y false
    #define DISPLAY_SWAP_XY true
    #define DISPLAY_OFFSET_X  DISPLAY_PHYSICAL_OFFSET
    #define DISPLAY_OFFSET_Y  0

#else
    #error "DISPLAY_ROTATION must be 0, 90, 180, or 270"
#endif

#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_2
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT true


#endif // _BOARD_CONFIG_H_
