// TO DO:
// Animated intro.
// Colored bars grow to the right
// digits pop up one by one going down or randomly
// Add a config page to adjust the bar width
// If bluetooth state changes during use just use static colour change. Later animate it, e.g. black up then colour down.

#include <pebble.h>

Window *my_window;
Layer *background_static, *background_dynamic;
Layer *hour1_layer, *hour2_layer, *min1_layer, *min2_layer;

// Colours
GColor color_bg, color_text;

typedef struct {
    int R;
    int G;
    int B;
} ColorRGB;

// 10 battery levels by 8 coloured bars
ColorRGB fadeArrayBTOn[10][8] = {
    /* 10% */ {{255, 0, 0}, {255, 0, 0}, {170, 0, 0}, {170, 0, 0}, {85, 0, 0}, {85, 0, 0}, {0, 0, 170}, {0, 0, 170}},
    /* 20% */ {{255, 0, 0}, {255, 0, 0}, {170, 0, 0}, {170, 0, 0}, {85, 0, 0}, {85, 0, 0}, {0, 0, 170}, {0, 0, 170}},
    /* 30% */ {{255, 0, 0}, {255, 0, 0}, {170, 0, 0}, {170, 0, 0}, {85, 0, 0}, {85, 0, 0}, {0, 0, 170}, {0, 0, 170}},
    /* 40% */ {{255, 255, 0}, {255, 255, 0}, {255, 170, 0}, {255, 170, 0}, {255, 85, 0}, {255, 85, 0}, {0, 0, 170}, {0, 0, 170}},
    /* 50% */ {{255, 255, 0}, {255, 255, 0}, {255, 170, 0}, {255, 170, 0}, {255, 85, 0}, {255, 85, 0}, {0, 0, 170}, {0, 0, 170}},
    /* 60% */ {{255, 255, 0}, {255, 255, 0}, {255, 170, 0}, {255, 170, 0}, {255, 85, 0}, {255, 85, 0}, {0, 0, 170}, {0, 0, 170}},
    /* 70% */ {{85, 255, 0}, {85, 255, 0}, {85, 170, 0}, {85, 170, 0}, {0, 85, 0}, {0, 85, 0}, {0, 0, 170}, {0, 0, 170}},
    /* 80% */ {{85, 255, 0}, {85, 255, 0}, {85, 170, 0}, {85, 170, 0}, {0, 85, 0}, {0, 85, 0}, {0, 0, 170}, {0, 0, 170}},
    /* 90% */ {{85, 255, 0}, {85, 255, 0}, {85, 170, 0}, {85, 170, 0}, {0, 85, 0}, {0, 85, 0}, {0, 0, 170}, {0, 0, 170}},
    /* 100% */{{85, 255, 0}, {85, 255, 0}, {85, 170, 0}, {85, 170, 0}, {0, 85, 0}, {0, 85, 0}, {0, 0, 170}, {0, 0, 170}}
};
ColorRGB fadeArrayBTOff[10][8] = {
    /* 10% */ {{255, 0, 0}, {255, 0, 0}, {170, 0, 0}, {170, 0, 0}, {170, 0, 0}, {170, 0, 0}, {85, 0, 0},{85, 0, 0}},
    /* 20% */ {{255, 0, 0}, {255, 0, 0}, {170, 0, 0}, {170, 0, 0}, {170, 0, 0}, {170, 0, 0}, {85, 0, 0},{85, 0, 0}},
    /* 30% */ {{255, 0, 0}, {255, 0, 0}, {170, 0, 0}, {170, 0, 0}, {170, 0, 0}, {170, 0, 0}, {85, 0, 0},{85, 0, 0}},
    /* 40% */ {{255, 255, 0}, {255, 255, 0}, {255, 170, 0}, {255, 170, 0}, {255, 170, 0}, {255, 170, 0}, {255, 85, 0}, {255, 85, 0}},
    /* 50% */ {{255, 255, 0}, {255, 255, 0}, {255, 170, 0}, {255, 170, 0}, {255, 170, 0}, {255, 170, 0}, {255, 85, 0}, {255, 85, 0}},
    /* 60% */ {{255, 255, 0}, {255, 255, 0}, {255, 170, 0}, {255, 170, 0}, {255, 170, 0}, {255, 170, 0}, {255, 85, 0}, {255, 85, 0}},
    /* 70% */ {{85, 255, 0}, {85, 255, 0}, {85, 170, 0}, {85, 170, 0},  {85, 170, 0}, {85, 170, 0}, {0, 85, 0}, {0, 85, 0}},
    /* 80% */ {{85, 255, 0}, {85, 255, 0}, {85, 170, 0}, {85, 170, 0},  {85, 170, 0}, {85, 170, 0}, {0, 85, 0}, {0, 85, 0}},
    /* 90% */ {{85, 255, 0}, {85, 255, 0}, {85, 170, 0}, {85, 170, 0},  {85, 170, 0}, {85, 170, 0}, {0, 85, 0}, {0, 85, 0}},
    /* 100% */{{85, 255, 0}, {85, 255, 0}, {85, 170, 0}, {85, 170, 0},  {85, 170, 0}, {85, 170, 0}, {0, 85, 0}, {0, 85, 0}}
};

// Digits for the time
int hour1, hour2, min1, min2;

// Dimensions of all the screen components
int bars_color_num;
int bars_color_lef_x, bars_color_lef_y, bars_color_lef_w, bars_color_lef_h;
int bars_color_rig_x, bars_color_rig_y, bars_color_rig_w, bars_color_rig_h;
int bars_color_gap, bars_color_incr;
int	digit_bar_x;
int	digit_bar_y;
int	digit_bar_h;
int	digit_bar_w;
int digit_padding;

// Init all the dimensions
// Use this later to set all the config variables
void init_dimensions() {
    digit_bar_w = 40;
    digit_bar_h = 168;
    digit_bar_y = 0;
    digit_bar_x = 144/2-digit_bar_w/2;

    bars_color_num = 8;

    bars_color_lef_x = 0;
    bars_color_lef_y = 0;
    bars_color_lef_w = digit_bar_x + digit_bar_w/2;
    bars_color_lef_h = 14;

    bars_color_rig_x = bars_color_lef_x + bars_color_lef_w;
    bars_color_rig_y = bars_color_lef_y;
    bars_color_rig_w = 144 - (bars_color_lef_x + bars_color_lef_w);
    bars_color_rig_h = bars_color_lef_h;

    bars_color_gap = (168 - bars_color_num*bars_color_lef_h)/(bars_color_num-1);
    bars_color_incr = 0;
    digit_padding = 8;

    color_text = GColorWhite;
    color_bg = GColorBlack;
}

// The functions used to draw each stroke of a digit
void draw_stroke_top_full(GContext *ctx, GRect bounds) {
    graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, digit_padding), 0, GCornerNone);
}

void draw_stroke_bottom_full(GContext *ctx, GRect bounds) {
    graphics_fill_rect(ctx, GRect(0, bounds.size.h-digit_padding, bounds.size.w, digit_padding), 0, GCornerNone);
}

void draw_stroke_middle_full(GContext *ctx, GRect bounds) {
    graphics_fill_rect(ctx, GRect(0, bounds.size.h/2-digit_padding/2, bounds.size.w, digit_padding), 0, GCornerNone);
}

void draw_stroke_left_full(GContext *ctx, GRect bounds) {
    graphics_fill_rect(ctx, GRect(0, 0, digit_padding, bounds.size.h), 0, GCornerNone);
}

void draw_stroke_left_upr(GContext *ctx, GRect bounds) {
    graphics_fill_rect(ctx, GRect(0, 0, digit_padding, bounds.size.h/2+digit_padding/2), 0, GCornerNone);
}

void draw_stroke_left_lwr(GContext *ctx, GRect bounds) {
    graphics_fill_rect(ctx, GRect(0, bounds.size.h/2-digit_padding/2, digit_padding, bounds.size.h/2+digit_padding/2), 0, GCornerNone);
}

void draw_stroke_middle_full_down(GContext *ctx, GRect bounds) {
    graphics_fill_rect(ctx, GRect(bounds.size.w/2-digit_padding/2, 0, digit_padding, bounds.size.h), 0, GCornerNone);
}

void draw_stroke_right_full(GContext *ctx, GRect bounds) {
    graphics_fill_rect(ctx, GRect(bounds.size.w-digit_padding, 0, digit_padding, bounds.size.h), 0, GCornerNone);
}

void draw_stroke_right_upr(GContext *ctx, GRect bounds) {
    graphics_fill_rect(ctx, GRect(bounds.size.w-digit_padding, 0, digit_padding, bounds.size.h/2+digit_padding/2), 0, GCornerNone);
}

void draw_stroke_right_lwr(GContext *ctx, GRect bounds) {
    graphics_fill_rect(ctx, GRect(bounds.size.w-digit_padding, bounds.size.h/2-digit_padding/2, digit_padding, bounds.size.h/2+digit_padding/2), 0, GCornerNone);
}

// Draw the digits
void draw_digit(GContext *ctx, Layer *me, int digit) {
    GRect bounds = layer_get_bounds(me);

    //digit = -1;

    if(digit>=0) {
        graphics_context_set_fill_color(ctx, color_bg);
        graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, bounds.size.h), 0, GCornerNone);
    }

    graphics_context_set_fill_color(ctx, color_text);

    switch(digit) {
        case 0:
            draw_stroke_top_full(ctx, bounds);
            draw_stroke_left_full(ctx, bounds);
            draw_stroke_right_full(ctx, bounds);
            draw_stroke_bottom_full(ctx, bounds);
            break;
        case 1:
            draw_stroke_middle_full_down(ctx, bounds);
            break;
        case 2:
            draw_stroke_top_full(ctx, bounds);
            draw_stroke_right_upr(ctx, bounds);
            draw_stroke_middle_full(ctx, bounds);
            draw_stroke_left_lwr(ctx, bounds);
            draw_stroke_bottom_full(ctx, bounds);
            break;
        case 3:
            draw_stroke_top_full(ctx, bounds);
            draw_stroke_right_upr(ctx, bounds);
            draw_stroke_middle_full(ctx, bounds);
            draw_stroke_right_lwr(ctx, bounds);
            draw_stroke_bottom_full(ctx, bounds);
            break;
        case 4:
            draw_stroke_left_upr(ctx, bounds);
            draw_stroke_middle_full(ctx, bounds);
            draw_stroke_right_full(ctx, bounds);
            break;
        case 5:
            draw_stroke_top_full(ctx, bounds);
            draw_stroke_left_upr(ctx, bounds);
            draw_stroke_middle_full(ctx, bounds);
            draw_stroke_right_lwr(ctx, bounds);
            draw_stroke_bottom_full(ctx, bounds);
            break;
        case 6:
            draw_stroke_top_full(ctx, bounds);
            draw_stroke_left_full(ctx, bounds);
            draw_stroke_middle_full(ctx, bounds);
            draw_stroke_right_lwr(ctx, bounds);
            draw_stroke_bottom_full(ctx, bounds);
            break;
        case 7:
            draw_stroke_top_full(ctx, bounds);
            draw_stroke_right_full(ctx, bounds);
            break;
        case 8:
            draw_stroke_top_full(ctx, bounds);
            draw_stroke_left_full(ctx, bounds);
            draw_stroke_middle_full(ctx, bounds);
            draw_stroke_right_full(ctx, bounds);
            draw_stroke_bottom_full(ctx, bounds);
            break;
        case 9:
            draw_stroke_top_full(ctx, bounds);
            draw_stroke_left_upr(ctx, bounds);
            draw_stroke_middle_full(ctx, bounds);
            draw_stroke_right_full(ctx, bounds);
            draw_stroke_bottom_full(ctx, bounds);
            break;
        default:
            // Draw nothing. This is used in case we are stripping the leading zero
        break;
    }

}

// Pass in the layer bounds
// Draws hour1
void draw_hour1(Layer *me, GContext *ctx) {
    draw_digit(ctx, me, hour1);
}

// Draws hour2
void draw_hour2(Layer *me, GContext *ctx) {
    draw_digit(ctx, me, hour2);
}

// Draws min1
void draw_min1(Layer *me, GContext *ctx) {
    draw_digit(ctx, me, min1);
}

// Draws min2
void draw_min2(Layer *me, GContext *ctx) {
    draw_digit(ctx, me, min2);
}

// Draws the static elements of the background
void draw_background_static(Layer *me, GContext *ctx) {
    // Background then Digits bar
    graphics_context_set_fill_color(ctx, color_bg);
    graphics_fill_rect(ctx, layer_get_bounds(me), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(digit_bar_x,digit_bar_y, digit_bar_w, digit_bar_h), 0, GCornerNone);
}

// Draws the dynamic elements of the background
void draw_background_dynamic(Layer *me, GContext *ctx) {
    int i;
    int R, G, B;

    // Take a peek at the battery state
    BatteryChargeState charge_state = battery_state_service_peek();
    int charge = charge_state.charge_percent;
    //charge = 20;
    charge = charge/10 -1;

    for(i=0; i<bars_color_num; i++) {
        if(bluetooth_connection_service_peek()) {
            R = fadeArrayBTOn[charge][i].R;
            G = fadeArrayBTOn[charge][i].G;
            B = fadeArrayBTOn[charge][i].B;
        } else {
            R = fadeArrayBTOff[charge][i].R;
            G = fadeArrayBTOff[charge][i].G;
            B = fadeArrayBTOff[charge][i].B;
        }

        graphics_context_set_fill_color(ctx, GColorFromRGB(R, G, B));
        graphics_fill_rect(ctx, GRect(bars_color_lef_x, bars_color_lef_y + i*(bars_color_lef_h+bars_color_gap), bars_color_lef_w + bars_color_incr, bars_color_lef_h), 0, GCornerNone);
        //graphics_context_set_fill_color(ctx, GColorFromRGB(255, 255, 255));
        graphics_fill_rect(ctx, GRect(bars_color_rig_x, bars_color_rig_y + i*(bars_color_rig_h+bars_color_gap), bars_color_rig_w + bars_color_incr, bars_color_rig_h), 0, GCornerNone);
    }
}

// Bluetooth handler
void handle_bluetooth(bool connected) {
    // Redraw the dynamic background layer
    layer_mark_dirty(background_dynamic);
}

// Battery handler
void handle_battery(BatteryChargeState charge_state) {
    // Redraw the dynamic background layer
    layer_mark_dirty(background_dynamic);
}

// Update the digits
void update_time(struct tm *tick_time) {

    if(clock_is_24h_style() == true) {
        hour1 = tick_time->tm_hour/10;
        hour2 = tick_time->tm_hour%10;
    } else {
        // Convert to 12h style and strip the leading zero
        hour1 = tick_time->tm_hour;
        //hour1 = 3;
        if(hour1==0) {
            hour1 = 1;
            hour2 = 2;
        } else if (hour1>=1 && hour1<=9) {
            hour2 = hour1%10;
            hour1 = -1;
        } else if (hour1>=10 && hour1<=12) {
            hour2 = hour1%10;
            hour1 = 1;
        } else if (hour1>=13 && hour1<=21) {
            hour1 -=12;
            hour2 = hour1%10;
            hour1 = -1;
        } else {
            hour1 -=12;
            hour2 = hour1%10;
            hour1 = 1;
        }
    }
    min1 = tick_time->tm_min/10;
    min2 = tick_time->tm_min%10;
    layer_mark_dirty(hour1_layer);
    layer_mark_dirty(hour2_layer);
    layer_mark_dirty(min1_layer);
    layer_mark_dirty(min2_layer);
}

// Ticker handler
void handle_tick(struct tm *tick_time, TimeUnits units) {
    update_time(tick_time);
}

void main_window_load() {
    Layer *window_layer = window_get_root_layer(my_window);
    GRect bounds = layer_get_bounds(window_layer);

    background_static = layer_create(bounds);
    layer_set_update_proc(background_static, draw_background_static);

    background_dynamic = layer_create(bounds);
    layer_set_update_proc(background_dynamic, draw_background_dynamic);

    hour1_layer = layer_create(GRect(digit_bar_x, 0, digit_bar_w, 2*bars_color_lef_h+bars_color_gap));
    layer_set_update_proc(hour1_layer, draw_hour1);

    hour2_layer = layer_create(GRect(digit_bar_x, 2*bars_color_lef_h+2*bars_color_gap, digit_bar_w, 2*bars_color_lef_h+bars_color_gap));
    layer_set_update_proc(hour2_layer, draw_hour2);

    min1_layer = layer_create(GRect(digit_bar_x, 4*bars_color_lef_h+4*bars_color_gap, digit_bar_w, 2*bars_color_lef_h+bars_color_gap));
    layer_set_update_proc(min1_layer, draw_min1);

    min2_layer = layer_create(GRect(digit_bar_x, 6*bars_color_lef_h+6*bars_color_gap, digit_bar_w, 2*bars_color_lef_h+bars_color_gap));
    layer_set_update_proc(min2_layer, draw_min2);

    layer_add_child(window_layer, background_static);
    layer_add_child(window_layer, background_dynamic);
    layer_add_child(window_layer, hour1_layer);
    layer_add_child(window_layer, hour2_layer);
    layer_add_child(window_layer, min1_layer);
    layer_add_child(window_layer, min2_layer);
}

void main_window_unload() {
    layer_destroy(background_static);
    layer_destroy(background_dynamic);
    layer_destroy(hour1_layer);
    layer_destroy(hour2_layer);
    layer_destroy(min1_layer);
    layer_destroy(min2_layer);
}

void init() {
    my_window = window_create();

    // Subscribe to bluetooth
    bluetooth_connection_service_subscribe(handle_bluetooth);

    // Subcribe to the battery
    battery_state_service_subscribe(handle_battery);

    // Init all the dimensions of the shapes to draw
    init_dimensions();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(my_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    window_stack_push(my_window, true);

    // Time for display on start up
    time_t temp_time = time(NULL);
    struct tm *tick_time = localtime(&temp_time);
    update_time(tick_time);

    // Subcribe to ticker
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

void deinit() {
    window_destroy(my_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}