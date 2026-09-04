/**
 * @file wd_arcoiris.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_arcoiris_private_gen.h"
#include "../../helios_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static uint32_t arc_colors[] = {

    0xB70074, /* Purple */
    0x792672, /* Violet */
    0x193E8D, /* Deep Blue */
    0x179DD6, /* Light Blue */
    0x009059, /* Dark Green */
    0x5FB136, /* Light Green */
    0xF9EE19, /* Yellow */
    0xF6D10E, /* Ocher */
    0xEF7916, /* Cream */

    0xEA4427, /* Orange */
    0xEB0F13, /* Red */
    0xD50059, /* Garnet */
};

/**********************
 *      MACROS
 **********************/
#define ARC_POS(i) ((i) * 30 - 15)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void wd_arcoiris_constructor_hook(lv_obj_t * obj)
{

    for(uint32_t i = 0; i < 12; i++) {
        lv_obj_t * arc = lv_arc_create(obj);
        lv_obj_remove_style_all(arc);
        lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
        lv_obj_set_align(arc, LV_ALIGN_CENTER);
        lv_arc_set_bg_start_angle(arc, 0);
        lv_arc_set_bg_end_angle(arc, 30);
        lv_arc_set_rotation(arc, ARC_POS(i));
        lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE);

        lv_obj_set_style_arc_width(arc, lv_pct(5), 0);
        lv_obj_set_style_arc_rounded(arc, false, 0);
        lv_obj_set_style_arc_color(arc, lv_color_hex(arc_colors[i]), 0);
        lv_obj_set_style_arc_opa(arc, 255, 0);

    }

}

void wd_arcoiris_destructor_hook(lv_obj_t * obj)
{

}

void wd_arcoiris_event_hook(lv_event_t * e)
{

}

void wd_arcoiris_set_thickness(lv_obj_t * wd_arcoiris, int32_t thickness)
{
    for (int32_t i = 0; i < lv_obj_get_child_count(wd_arcoiris); i++)
    {
        lv_obj_t * arc = lv_obj_get_child(wd_arcoiris, i);
        lv_obj_set_style_arc_width(arc, thickness, 0);
    }

}

/**********************
 *   STATIC FUNCTIONS
 **********************/