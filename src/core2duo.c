#include <pebble.h>

static Window *main_window;

static BitmapLayer *cpu_layer;
static GBitmap *cpu_bitmap;

static TextLayer *date_layer;
static TextLayer *time_layer;

static void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    static char date_buffer[11];
    strftime(date_buffer, sizeof(date_buffer), "%d/%m/%y", tick_time);

    static char time_buffer[8];
    strftime(time_buffer, sizeof(time_buffer), clock_is_24h_style() ?
                                            "%H:%M" : "%I:%M", tick_time);
    // remove leading zero if 12h format
    char *time_buffer_ptr = time_buffer;
    if (time_buffer[0] == '0' && !clock_is_24h_style()) {
        time_buffer_ptr++;
    }

    text_layer_set_text(date_layer, date_buffer);
    text_layer_set_text(time_layer, time_buffer_ptr);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) { update_time(); }

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
    
    cpu_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CPU);
    cpu_layer = bitmap_layer_create(bounds);

    bitmap_layer_set_bitmap(cpu_layer, cpu_bitmap);

    // position date and time text
    date_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(130,125), bounds.size.w, 40));
    
    time_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(10,3), bounds.size.w, 40));
    
    // date
    text_layer_set_text_color(date_layer, GColorBlack);
    text_layer_set_text(date_layer, "20/03/25");
    text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);

    // time
    text_layer_set_text_color(time_layer, GColorBlack);
    text_layer_set_text(time_layer, "12:45");
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
    
    // display everything to the screen
    layer_add_child(window_layer, bitmap_layer_get_layer(cpu_layer));
    layer_add_child(window_layer, text_layer_get_layer(date_layer));
    layer_add_child(window_layer, text_layer_get_layer(time_layer));
}

static void window_unload(Window *window) {
    text_layer_destroy(date_layer);
    text_layer_destroy(time_layer);
    gbitmap_destroy(cpu_bitmap);
    bitmap_layer_destroy(cpu_layer);
}

static void init() {
    main_window = window_create();

    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload
    });

    window_stack_push(main_window, true);

    update_time();
    
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
    window_destroy(main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
