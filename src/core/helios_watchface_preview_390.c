#include "helios_ui/custom/watchfaces/watchface_manager.h"

extern const lv_image_dsc_t img_preview_390_data;

const void * img_preview_390 = &img_preview_390_data;

static void helios_default_390_preview_register(void)
{
    const helios_watchface_preview_t previews[] = {
        { 390, 450, img_preview_390 },
    };

    helios_watchfaces_set_previews("default", previews, 1);
}

HELIOS_REGISTER_WATCHFACE_INITIALIZER(helios_default_390_preview_register)
