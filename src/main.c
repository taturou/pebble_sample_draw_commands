#include <pebble.h>

static Window *s_window;
static Layer *s_layer;
static GDrawCommandImage *s_image[7];
static int image_num = 0;
static float power = 1;

static void s_layer_update_proc(struct Layer *layer, GContext *ctx) {
    GDrawCommandImage *image = gdraw_command_image_clone(s_image[image_num]);
    GDrawCommandList *cmdlist = gdraw_command_image_get_command_list(image);

    uint16_t num_cmd = (uint16_t)gdraw_command_list_get_num_commands(cmdlist);
    for (uint16_t i = 0; i < num_cmd; i++) {
        GDrawCommand *cmd = gdraw_command_list_get_command(cmdlist, i);
        gdraw_command_set_stroke_color(cmd, (GColor8){.argb=((uint8_t)((i*2)%64) | (uint8_t)0b11000000)});

        uint16_t num_point = gdraw_command_get_num_points(cmd);
        for (uint16_t j = 0; j < num_point; j++) {
            GPoint p = gdraw_command_get_point(cmd, j);
            gdraw_command_set_point(cmd, j, GPoint((uint16_t)(p.x * power), (uint16_t)(p.y * power)));
        }
    }

    gdraw_command_image_draw(ctx, image, GPoint(0,0));
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    image_num = (image_num + 1) % 7;
    layer_mark_dirty(s_layer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    power += 0.1;
    layer_mark_dirty(s_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    power -= 0.1;
    layer_mark_dirty(s_layer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // layer
    s_layer = layer_create(bounds);
    layer_set_update_proc(s_layer, s_layer_update_proc);
    layer_add_child(window_layer, s_layer);

    //
    s_image[0] = gdraw_command_image_create_with_resource(RESOURCE_ID_ICON_GENERIC_WEATHER);
    s_image[1] = gdraw_command_image_create_with_resource(RESOURCE_ID_ICON_HEAVY_RAIN);
    s_image[2] = gdraw_command_image_create_with_resource(RESOURCE_ID_ICON_HEAVY_SNOW);
    s_image[3] = gdraw_command_image_create_with_resource(RESOURCE_ID_ICON_LIGHT_RAIN);
    s_image[4] = gdraw_command_image_create_with_resource(RESOURCE_ID_ICON_LIGHT_SNOW);
    s_image[5] = gdraw_command_image_create_with_resource(RESOURCE_ID_ICON_PARTLY_CLOUDY);
    s_image[6] = gdraw_command_image_create_with_resource(RESOURCE_ID_ICON_SUNNY_DAY);
}

static void window_unload(Window *window) {
}

static void init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  deinit();
}
