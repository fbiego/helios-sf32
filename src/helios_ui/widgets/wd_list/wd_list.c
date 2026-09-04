/**
 * @file wd_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "wd_list_private_gen.h"
#include "../../helios_ui.h"

/*********************
 *      DEFINES
 *********************/

#define ARC_BG_START        -20
#define ARC_BG_END          20
#define ARC_ELASTIC_RANGE   20

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void on_list_event_cb(lv_event_t *e);
static void on_scroll_cb(lv_event_t *e);
static void on_child_changed_cb(lv_event_t *e);
static void draw_circular_scrollbar_event(lv_event_t * e);
static int32_t normalize_angle(int32_t angle);

static void mode_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void circular_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void screen_observer_cb(lv_observer_t * observer, lv_subject_t * subject);

static void delay_timer_cb(lv_timer_t * t);


/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void wd_list_constructor_hook(lv_obj_t * obj)
{

    wd_list_t * widget = (wd_list_t *)obj;

    lv_obj_set_scroll_dir(widget->list, LV_DIR_VER);

    // lv_obj_add_event_cb(widget->list, on_list_event_cb, LV_EVENT_ALL, widget);
    lv_obj_add_event_cb(widget->list, on_scroll_cb, LV_EVENT_SCROLL, widget);
    lv_obj_add_event_cb(widget->list, draw_circular_scrollbar_event, LV_EVENT_DRAW_POST, widget);
    lv_obj_add_event_cb(widget->list, on_child_changed_cb, LV_EVENT_CHILD_CHANGED, widget);
    

}

void wd_list_destructor_hook(lv_obj_t * obj)
{

}

void wd_list_event_hook(lv_event_t * e)
{

}

void wd_list_bind_mode(lv_obj_t * wd_list, lv_subject_t * bind_mode)
{
    wd_list_t * widget = (wd_list_t *)wd_list;
    widget->bind_mode = bind_mode;
    lv_subject_add_observer_obj(widget->bind_mode, mode_observer_cb, wd_list, widget);
    lv_obj_send_event(widget->list, LV_EVENT_SCROLL, widget);
}

void wd_list_bind_circular(lv_obj_t * wd_list, lv_subject_t * bind_circular)
{
    wd_list_t * widget = (wd_list_t *)wd_list;
    widget->bind_circular = bind_circular;
    lv_subject_add_observer_obj(widget->bind_circular, circular_observer_cb, wd_list, widget);
    lv_obj_send_event(widget->list, LV_EVENT_SCROLL, widget);
}

void wd_list_bind_screen(lv_obj_t * wd_list, lv_subject_t * bind_screen)
{
    wd_list_t * widget = (wd_list_t *)wd_list;
    widget->bind_screen = bind_screen;
    lv_subject_add_observer_obj(widget->bind_screen, screen_observer_cb, wd_list, widget);
}

void wd_list_set_center_lock(lv_obj_t * wd_list, bool center_lock)
{
    wd_list_t * widget = (wd_list_t *)wd_list;
    widget->center_lock = center_lock;

}

lv_obj_t * wd_list_get_container(lv_obj_t * wd_list)
{
    wd_list_t * widget = (wd_list_t *)wd_list;
    return widget->list;
}

lv_obj_t * wd_list_get_title(lv_obj_t * wd_list)
{
    wd_list_t * widget = (wd_list_t *)wd_list;
    return widget->title;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
static void on_child_changed_cb(lv_event_t *e)
{
    wd_list_t * widget = lv_event_get_user_data(e);
    lv_obj_send_event(widget->list, LV_EVENT_SCROLL, widget);
}

static void on_list_event_cb(lv_event_t *e)
{
    wd_list_t * widget = lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * cont = lv_event_get_target_obj(e);
    lv_indev_t * indev = lv_indev_active();


    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    int32_t cont_width = lv_area_get_width(&cont_a);
    int32_t cont_x_center = cont_a.x1 + cont_width / 2;

    if (code == LV_EVENT_PRESSED) {
        if(indev == NULL)  return;
        lv_point_t p;
        lv_indev_get_point(indev, &p);

        int32_t diff_x = LV_ABS(cont_x_center - p.x);
        if (diff_x < cont_width / 5 && widget->center_lock) {
            LV_LOG_USER("Locking list, diff_x %d, cont_width %d", diff_x, cont_width / 5 );
            lv_obj_set_flag(widget->list, LV_OBJ_FLAG_SCROLLABLE, false);
            lv_obj_set_flag(widget->list, LV_OBJ_FLAG_EVENT_BUBBLE, true);
        }
        // LV_LOG_USER("List touched at X %d, center %d", p.x, cont_x_center);

    }
    if (code == LV_EVENT_RELEASED) {
        lv_obj_set_flag(widget->list, LV_OBJ_FLAG_SCROLLABLE, true);
        lv_obj_set_flag(widget->list, LV_OBJ_FLAG_EVENT_BUBBLE, false);
    }
}

static void delay_timer_cb(lv_timer_t * t)
{
    wd_list_t * widget = lv_timer_get_user_data(t);
    LV_LOG_USER("delay_timer_cb WIDGET %p", widget);
    lv_obj_send_event(widget->list, LV_EVENT_SCROLL, widget);
}

static void mode_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    wd_list_t * widget = lv_observer_get_user_data(observer);

    bool mode = lv_subject_get_int(subject);
    bool circular = false;
    if (widget->bind_circular) {
        circular = lv_subject_get_int(widget->bind_circular);
    }

    if (!mode && circular) {
        /* Use a timer to trigger delayed event */
        lv_timer_t * timer = lv_timer_create(delay_timer_cb, 50, widget);
        lv_timer_set_repeat_count(timer, 1);
    } else {
        lv_obj_send_event(widget->list, LV_EVENT_SCROLL, widget);
    }

}

static void circular_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    wd_list_t * widget = lv_observer_get_user_data(observer);

    bool circular = lv_subject_get_int(subject);

    lv_obj_send_event(widget->list, LV_EVENT_SCROLL, widget);

}

static void screen_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{

}

static void on_scroll_cb(lv_event_t *e)
{
    wd_list_t * widget = lv_event_get_user_data(e);
    lv_obj_t * cont = widget->list;

    // LV_LOG_USER("WD list %p scroll event", widget);

    bool circular = false;
    bool mode = false;
    if (widget->bind_circular) {
        circular = lv_subject_get_int(widget->bind_circular);
    }
    if (widget->bind_mode) {
        mode = lv_subject_get_int(widget->bind_mode);
    }
    if (mode) {
        /* Disable circular scroll in grid mode */
        circular = false;
    }

    // LV_LOG_USER("WD List %p Circular %d", widget, circular);

    /* START Title reposition section */

    int32_t pad_top = lv_obj_get_style_pad_top(widget->list, LV_PART_MAIN);
    int32_t pad_left = lv_obj_get_style_pad_left(widget->list, LV_PART_MAIN);
    int32_t pad_right = lv_obj_get_style_pad_right(widget->list, LV_PART_MAIN);
    int32_t pad_row= lv_obj_get_style_pad_row(widget->list, LV_PART_MAIN);

    int32_t scroll_top = lv_obj_get_scroll_top(widget->list);

    lv_area_t cont_tl;
    lv_obj_get_coords(widget->title, &cont_tl);

    int32_t title_h = lv_area_get_height(&cont_tl) - lv_obj_get_style_pad_top(widget->title, LV_PART_MAIN);
    int32_t opa_map = lv_map(scroll_top, 0, pad_top - pad_row, 255, 0);
    int32_t new_pad = pad_top - (title_h + pad_row) - scroll_top;

    // LV_LOG_USER("Pad top: %d, title_h %d, scroll_top %d, new_pad %d, opa_map %d", pad_top, title_h, scroll_top, pad_top - (title_h + pad_row) - scroll_top, opa_map);

    if (new_pad < pad_row) new_pad = pad_row;

    lv_obj_set_style_pad_left(widget->title, pad_left, 0);
    lv_obj_set_style_pad_right(widget->title, pad_right, 0);
    // lv_obj_set_style_pad_top(widget->title, pad_top - (title_h + pad_row), 0);
    lv_obj_set_y(widget->title, new_pad);
    lv_obj_set_style_opa(widget->title, opa_map, 0);
    /* END Title reposition section */


    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    int32_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    int32_t r = lv_obj_get_height(cont) * 7 / 10;
    int32_t w = lv_obj_get_width(cont);
    int32_t i;
    int32_t child_cnt = (int32_t)lv_obj_get_child_count(cont);
    // LV_LOG_USER("WD List %p ch %d r %d w %d ctr %d ", widget, child_cnt, r, w, cont_y_center);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        int32_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);
 
        

        /*Get the x of diff_y on a circle.*/
        int32_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if(diff_y >= r) {
            x = r;
        }
        else {
            /*Use Pythagoras theorem to get x from radius and y*/
            uint32_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }

        if (mode) {
            // x = lv_map(x, 0, r, -10, 100);
            // if (i % 3 == 0) {
            //     lv_obj_set_style_translate_x(child, circular ? x : 0, 0);
            // } else if ((i % 3) == 1) {
            //     lv_obj_set_style_translate_x(child, 0, 0);
            // } else if ((i % 3) == 2) {
            //     lv_obj_set_style_translate_x(child, circular ? -x : 0, 0);
            // }
            lv_obj_set_style_translate_x(child, circular ? x : 0, 0);
        } else {
            /*Translate the item by the calculated X coordinate*/
            lv_obj_set_style_translate_x(child, circular ? x : 0, 0);
        }

        /*Use some opacity with larger translations*/
        // lv_opa_t opa = (lv_opa_t)lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        // lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}

static void draw_circular_scrollbar_event(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_layer_t * layer = lv_event_get_layer(e);
    wd_list_t * widget = lv_event_get_user_data(e);

    int32_t arc_width = 5;
    if (widget->bind_screen) {
        int32_t s = get_screen_width(lv_subject_get_int(widget->bind_screen));
        switch (s)
        {
            case 466: arc_width = 5; break;
            case 360: arc_width = 4; break;
            case 240: arc_width = 3; break;
        }
    }

    /* Get object geometry */
    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);

    int32_t w = lv_area_get_width(&coords);
    int32_t h = lv_area_get_height(&coords);

    lv_point_t ctr;
    ctr.x = coords.x1 + w / 2;
    ctr.y = coords.y1 + h / 2;

    int32_t radius = LV_MIN(w, h) / 2 - 6;

    /* Scroll info */
    int32_t scroll_top = lv_obj_get_scroll_top(obj);
    int32_t scroll_bottom = lv_obj_get_scroll_bottom(obj);
    int32_t scroll_range = scroll_top + scroll_bottom;
   

    if(scroll_range <= 0) return;

    /* Arc limits in extended angle space */
    int32_t bg_start = ARC_BG_START + 360;
    int32_t bg_end   = ARC_BG_END   + 360;
    int32_t bg_range = bg_end - bg_start;

    /* Compute raw center (before clamping) */
    int32_t raw_center =
        bg_start +
        (scroll_top * bg_range) / scroll_range;

    /* Detect overscroll */
    int32_t overscroll = 0;

    if(raw_center < bg_start)
        overscroll = bg_start - raw_center;
    else if(raw_center > bg_end)
        overscroll = raw_center - bg_end;

    /* Compute proportional indicator size */
    lv_obj_update_layout(obj);
    int32_t visible = lv_obj_get_height(obj);
    int32_t self = lv_obj_get_self_height(obj);
    int32_t content = lv_obj_get_content_height(obj);

    
    // LV_LOG_USER("visible %d, content %d, self %d", visible, content, self);

    content = 2;

    if(content <= 0) return;

    int32_t indicator_size =
        (visible * bg_range) / content;
    
    indicator_size = 6;

    /* Clamp to valid size */
    if(indicator_size < 1)
        indicator_size = 1;

    if(indicator_size > bg_range)
        indicator_size = bg_range;

    /* Apply elastic shrink */
    if(overscroll > 0)
    {
        int32_t shrink =
            (overscroll * (indicator_size - 1)) / ARC_ELASTIC_RANGE;

        indicator_size -= shrink;

        if(indicator_size < 1)
            indicator_size = 1;
    }

    int32_t half = indicator_size / 2;

    /* Clamp center so indicator stays inside arc */
    int32_t center = raw_center;

    if(center < bg_start + half)
        center = bg_start + half;

    if(center > bg_end - half)
        center = bg_end - half;

    /* Compute final angles */
    int32_t ind_start = normalize_angle(center - half);
    int32_t ind_end   = normalize_angle(center + half);


    int32_t type = lv_subject_get_int(&sb_screen_type);

    if (type == 0) {
        /* Draw background arc */
        lv_draw_arc_dsc_t arc_bg;
        lv_draw_arc_dsc_init(&arc_bg);

        arc_bg.rounded = true;
        arc_bg.center = ctr;
        arc_bg.radius = radius;
        arc_bg.start_angle = normalize_angle(ARC_BG_START);
        arc_bg.end_angle   = normalize_angle(ARC_BG_END);
        arc_bg.width = arc_width;
        arc_bg.color = lv_color_hex(0x444444);
        arc_bg.opa = LV_OPA_40;

        lv_draw_arc(layer, &arc_bg);

        /* Draw indicator arc */
        lv_draw_arc_dsc_t arc_ind;
        lv_draw_arc_dsc_init(&arc_ind);

        arc_ind.rounded = true;
        arc_ind.center = ctr;
        arc_ind.radius = radius;
        arc_ind.start_angle = ind_start;
        arc_ind.end_angle   = ind_end;
        arc_ind.width = arc_width;
        arc_ind.color = lv_color_white();
        arc_ind.opa = LV_OPA_60;

        lv_draw_arc(layer, &arc_ind);
    } else if (type == 1) {
        /* Background line */
        lv_draw_line_dsc_t bg_dsc;
        lv_draw_line_dsc_init(&bg_dsc);
        bg_dsc.color = lv_color_hex(0x444444);
        bg_dsc.width = arc_width;
        bg_dsc.opa   = LV_OPA_40;
        bg_dsc.round_start = 1;
        bg_dsc.round_end   = 1;

        bg_dsc.p1.x = coords.x2 - arc_width;
        bg_dsc.p2.x = coords.x2 - arc_width;
        bg_dsc.p1.y = coords.y1 +  (h * 0.3);
        bg_dsc.p2.y = coords.y1 +  (h * 0.7);

        lv_draw_line(layer, &bg_dsc);

        /* Map arc space → line space */
        int32_t y_start = coords.y1 + (h * 0.3);
        int32_t y_end   = coords.y1 + (h * 0.7);
        int32_t track_len = y_end - y_start;

        /* Convert center from angle → position */
        int32_t center_y =
            y_start +
            ((center - bg_start) * track_len) / bg_range;

        /* Convert size */
        int32_t ind_len = (indicator_size * track_len) / bg_range;
        int32_t half_len = ind_len / 2;

        /* Compute endpoints */
        int32_t ind_y1 = center_y - half_len;
        int32_t ind_y2 = center_y + half_len;

        /* Clamp (same idea as arc) */
        if(ind_y1 < y_start) {
            ind_y1 = y_start;
            ind_y2 = y_start + ind_len;
        }
        if(ind_y2 > y_end) {
            ind_y2 = y_end;
            ind_y1 = y_end - ind_len;
        }

        /* Draw Line indicator */
        lv_draw_line_dsc_t ind_dsc;
        lv_draw_line_dsc_init(&ind_dsc);

        ind_dsc.color = lv_color_white();
        ind_dsc.width = arc_width;
        ind_dsc.opa   = LV_OPA_60;
        ind_dsc.round_start = 1;
        ind_dsc.round_end   = 1;

        int32_t x = coords.x2 - arc_width;

        ind_dsc.p1.x = x;
        ind_dsc.p2.x = x;
        ind_dsc.p1.y = ind_y1;
        ind_dsc.p2.y = ind_y2;

        lv_draw_line(layer, &ind_dsc);
       
    }

}

static int32_t normalize_angle(int32_t angle)
{
    while(angle < 0) angle += 360;
    while(angle >= 360) angle -= 360;
    return angle;
}