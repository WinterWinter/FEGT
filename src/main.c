#include <pebble.h>

#define KEY_TEMPERATURE 1
#define KEY_ICON 2
#define KEY_SCALE 3
#define KEY_SCALE_OPTION 4
#define KEY_STEPSGOAL 5

static Window *s_main_window;

static TextLayer *month_layer;
static TextLayer *day_layer;
static TextLayer *temperature_layer;

static TextLayer *step_layer;
static TextLayer *text_battery_layer;
static TextLayer *s_time_layer;
static TextLayer *o_time_layer;

static GFont s_time_font;
static GFont o_time_font;
static GFont all_font;

int steps;
int stepgoal = 10000;
int step_percent;

static Layer *step_bar;

static int battery_level;
static Layer *battery_layer;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static BitmapLayer *weather_layer;
static GBitmap *weather_bitmap = NULL;

static BitmapLayer *battery_life_layer;
static GBitmap *battery_life_bitmap = NULL;

static BitmapLayer *step_life_layer;
static GBitmap *step_life_bitmap = NULL;

static const uint32_t WEATHER_ICONS[] = {
  RESOURCE_ID_ClearDay, //0
  RESOURCE_ID_Cloudy, //1
  RESOURCE_ID_OvercastDay, //2
  RESOURCE_ID_Rain, //3
  RESOURCE_ID_RainSnow, //4
  RESOURCE_ID_Snow, //5
  RESOURCE_ID_Storm, //6
  RESOURCE_ID_Tornado, //7
};

static const uint32_t LIFE_ICONS[] = {
  RESOURCE_ID_0,
  RESOURCE_ID_10,  
  RESOURCE_ID_20,  
  RESOURCE_ID_30,  
  RESOURCE_ID_40,  
  RESOURCE_ID_50,  
  RESOURCE_ID_60,  
  RESOURCE_ID_70,  
  RESOURCE_ID_80,  
  RESOURCE_ID_90,  
  RESOURCE_ID_100, 
};


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  static char day_text[] = "00ABC";
  char *day_format;
  day_format = "%a%e";
  
  
  static char month_text[] = "ABC";
  char *month_format;
  month_format = "%b";
  

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  
  strftime(day_text, sizeof(day_text), day_format, tick_time);
  strftime(month_text, sizeof(month_text), month_format, tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(o_time_layer, s_buffer);
  text_layer_set_text(day_layer, day_text);
  text_layer_set_text(month_layer, month_text);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  if(tick_time->tm_min % 30 == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, 0, 0);
    app_message_outbox_send();
    }
}

static void handle_health(HealthEventType event, void *context) 
{
	time_t start = time_start_of_today();
	time_t end = time(NULL);
	HealthServiceAccessibilityMask mask = health_service_metric_accessible(HealthMetricStepCount, start, end);
  static char percent_buffer[32];
  
		
		if (mask & HealthServiceAccessibilityMaskAvailable) {
				//APP_LOG(APP_LOG_LEVEL_INFO, "Step data available!");
				steps = health_service_sum_today(HealthMetricStepCount);
        step_percent = (double)steps / stepgoal * 100;
      if (step_percent <= 100){
        snprintf(percent_buffer, sizeof(percent_buffer), "%d", step_percent);
        text_layer_set_text(step_layer, percent_buffer);
      }
      else {
        text_layer_set_text(step_layer, "100");
      }
        //APP_LOG(APP_LOG_LEVEL_INFO, "Step Percent: %d", step_percent);
				//APP_LOG(APP_LOG_LEVEL_INFO, "Steps: %d", steps);
		} else {
				//APP_LOG(APP_LOG_LEVEL_INFO, "Step data unavailable");
		}
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) 
{
  static char temperature_buffer[8];
  static char weather_layer_buffer[32];
  static char percent_buffer[32];
  
  Tuple *t = dict_read_first(iterator);

  int temperature;
  int finalTemp;
  int scale_option = persist_read_int(KEY_SCALE_OPTION);
  
  
  while(t != NULL) {
    switch(t->key) {
    case KEY_SCALE:
      if(strcmp(t->value->cstring, "F") == 0){
        persist_write_int(KEY_SCALE_OPTION, 0);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
      }
      else if(strcmp(t->value->cstring, "C") == 0){
        persist_write_int(KEY_SCALE_OPTION, 1);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
      }
      break;
    case KEY_TEMPERATURE:
      
      if(scale_option == 0){
      temperature = t->value->int32;
      finalTemp = (temperature - 273.15) * 1.8 + 32;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", finalTemp);
      }
      else if(scale_option == 1){
      temperature = t->value->int32;
      finalTemp = temperature - 273.15;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", finalTemp);
      }
      break;
      
      case KEY_ICON:
      
      if (weather_bitmap) {
        gbitmap_destroy(weather_bitmap);
      }

      weather_bitmap = gbitmap_create_with_resource(WEATHER_ICONS[t->value->int32]);
      bitmap_layer_set_bitmap(weather_layer, weather_bitmap);
      break;
    
      case KEY_STEPSGOAL:
      
      stepgoal = t->value->int16;
		  APP_LOG(APP_LOG_LEVEL_INFO, "stepgoal is %d", stepgoal);
		  persist_write_int(KEY_STEPSGOAL, stepgoal);   
      break;
  }

    t = dict_read_next(iterator);
  }
  
  step_percent = (double)steps / stepgoal * 100;
  
   if (step_percent <= 100){
        snprintf(percent_buffer, sizeof(percent_buffer), "%d", step_percent);
        text_layer_set_text(step_layer, percent_buffer);
      }
      else {
        text_layer_set_text(step_layer, "100");
      }
  
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
  text_layer_set_text(temperature_layer, weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void step_layer_update_callback(Layer *layer, GContext *ctx) 
{  
  if (step_life_bitmap) {
    gbitmap_destroy(step_life_bitmap);
  }
  
   if(step_percent < 10) {step_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[0]);}
   else if(step_percent >= 10 && step_percent < 20) {step_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[1]);}
   else if(step_percent >= 20 && step_percent < 30) {step_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[2]);} 
   else if(step_percent >= 30 && step_percent < 40) {step_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[3]);}
   else if(step_percent >= 40 && step_percent < 50) {step_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[4]);}
   else if(step_percent >= 50 && step_percent < 60) {step_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[5]);}
   else if(step_percent >= 60 && step_percent < 70) {step_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[6]);}
   else if(step_percent >= 70 && step_percent < 80) {step_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[7]);}
   else if(step_percent >= 80 && step_percent < 90) {step_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[8]);}
   else if(step_percent >= 90 && step_percent < 100) {step_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[9]);}
   else if(step_percent == 100) {step_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[10]);}

   bitmap_layer_set_bitmap(step_life_layer, step_life_bitmap);
}

static void load_step_bar(Layer *window_layer)
{
 	step_bar = layer_create(GRect(34, 149, 33, 11));
 	layer_set_update_proc(step_bar, &step_layer_update_callback);
  layer_add_child(window_layer, step_bar);
}


static void battery_callback(BatteryChargeState state) {
  battery_level = state.charge_percent;
  layer_mark_dirty(battery_layer);
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  
  static char battery_buffer[32];
  snprintf(battery_buffer, sizeof(battery_buffer), "%d", battery_level);
  text_layer_set_text(text_battery_layer, battery_buffer);
  
  if (battery_life_bitmap) {
    gbitmap_destroy(battery_life_bitmap);}
  
   if(battery_level < 10) {battery_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[0]);}
   else if(battery_level >= 10 && battery_level < 20) {battery_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[1]);}
   else if(battery_level >= 20 && battery_level < 30) {battery_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[2]);} 
   else if(battery_level >= 30 && battery_level < 40) {battery_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[3]);}
   else if(battery_level >= 40 && battery_level < 50) {battery_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[4]);}
   else if(battery_level >= 50 && battery_level < 60) {battery_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[5]);}
   else if(battery_level >= 60 && battery_level < 70) {battery_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[6]);}
   else if(battery_level >= 70 && battery_level < 80) {battery_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[7]);}
   else if(battery_level >= 80 && battery_level < 90) {battery_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[8]);}
   else if(battery_level >= 80 && battery_level < 100) {battery_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[9]);}
   else if(battery_level == 100) {battery_life_bitmap = gbitmap_create_with_resource(LIFE_ICONS[10]);}

   bitmap_layer_set_bitmap(battery_life_layer, battery_life_bitmap);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 25), bounds.size.w, 50));
  o_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 25), bounds.size.w, 50));
  month_layer = text_layer_create(GRect(0,1,54,30));
  day_layer = text_layer_create(GRect(92,2,54,25));
  temperature_layer = text_layer_create(GRect(104,112,40,25));
  step_layer = text_layer_create(GRect(-3,138,40,25));
  text_battery_layer = text_layer_create(GRect(69,138,40,25));
  weather_layer = bitmap_layer_create(GRect(2, 117, 37, 23));
  
  battery_life_layer = bitmap_layer_create(GRect(106, 149, 33, 11));
  step_life_layer = bitmap_layer_create(GRect(34, 149, 33, 11));
  
  //Time
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  //Time2
  text_layer_set_background_color(o_time_layer, GColorClear);
  text_layer_set_text_color(o_time_layer, GColorBlack);
  text_layer_set_text(o_time_layer, "00:00");
  text_layer_set_text_alignment(o_time_layer, GTextAlignmentCenter);

  //Month
  text_layer_set_background_color(month_layer, GColorClear);
  text_layer_set_text_color(month_layer, GColorWhite);
  text_layer_set_text_alignment(month_layer, GTextAlignmentCenter);
  
  //Day
  text_layer_set_background_color(day_layer, GColorClear);
  text_layer_set_text_color(day_layer, GColorWhite);
  text_layer_set_text_alignment(day_layer, GTextAlignmentCenter);
  
  //Temperature
  text_layer_set_background_color(temperature_layer, GColorClear);
  text_layer_set_text_color(temperature_layer, GColorWhite);
  text_layer_set_text(temperature_layer, "----");
  text_layer_set_text_alignment(temperature_layer, GTextAlignmentCenter);
  
  //Battery
  battery_layer = layer_create(GRect(106, 149, 33, 11));
  layer_set_update_proc(battery_layer, battery_update_proc);
  
  //Battery Text
  text_layer_set_background_color(text_battery_layer, GColorClear);
  text_layer_set_text_color(text_battery_layer, GColorWhite);
  text_layer_set_text_alignment(text_battery_layer, GTextAlignmentCenter);
 
  //Step Text
  text_layer_set_background_color(step_layer, GColorClear);
  text_layer_set_text_color(step_layer, GColorWhite);
  text_layer_set_text_alignment(step_layer, GTextAlignmentCenter);
  
  load_step_bar(window_layer);

  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Solid_40));
  o_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Outline_40));
  all_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(o_time_layer, o_time_font);
  text_layer_set_font(month_layer, all_font);
  text_layer_set_font(day_layer, all_font);
  text_layer_set_font(temperature_layer, all_font);
  text_layer_set_font(text_battery_layer, all_font);
  text_layer_set_font(step_layer, all_font);
  
  layer_add_child(window_layer, bitmap_layer_get_layer(battery_life_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(step_life_layer));

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(o_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(month_layer));
  layer_add_child(window_layer, text_layer_get_layer(day_layer));
  layer_add_child(window_layer, text_layer_get_layer(temperature_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(weather_layer));
  layer_add_child(window_get_root_layer(window), battery_layer);
  layer_add_child(window_layer, text_layer_get_layer(step_layer));
  layer_add_child(window_layer, text_layer_get_layer(text_battery_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(o_time_layer);
  text_layer_destroy(month_layer);
  text_layer_destroy(day_layer);
  text_layer_destroy(temperature_layer);
  text_layer_destroy(step_layer);
  
  layer_destroy(battery_layer);

  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(o_time_font);

  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  gbitmap_destroy(weather_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set the background color
  window_set_background_color(s_main_window, GColorBlack);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  health_service_events_subscribe(handle_health, NULL);
  
  // Register for battery level updates
  battery_state_service_subscribe(battery_callback);
  // Ensure battery level is displayed from the start
  battery_callback(battery_state_service_peek());

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  stepgoal = persist_read_int(KEY_STEPSGOAL);
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  app_message_open(128,128); 
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  //persist_write_int(KEY_STEPSGOAL, stepgoal);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}