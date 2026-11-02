#include "helios_ui/custom/watchfaces/watchface_manager.h"

#if defined(HELIOS_BOARD_SF32LB52_LCD_CUSTOM_466X466_CO5300)
extern const lv_image_dsc_t img_preview_466_data;

const void * img_preview_466 = &img_preview_466_data;
#else
extern const lv_image_dsc_t img_preview_390_data;

const void * img_preview_390 = &img_preview_390_data;
#endif

static void helios_default_preview_register(void)
{
    const helios_watchface_preview_t previews[] = {
#if defined(HELIOS_BOARD_SF32LB52_LCD_CUSTOM_466X466_CO5300)
        { 466, 466, img_preview_466 },
#else
        { 390, 450, img_preview_390 },
#endif
    };

    helios_watchfaces_set_previews("default", previews, 1);
}

HELIOS_REGISTER_WATCHFACE_INITIALIZER(helios_default_preview_register)
