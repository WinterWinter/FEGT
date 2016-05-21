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

static BitmapLayer *enemy_layer;
static GBitmap *enemy_bitmap;

static BitmapLayer *weapon_layer;
static GBitmap *weapon_bitmap;

static BitmapLayer *ui_layer;
static GBitmap *ui_bitmap;

static BitmapLayer *weather_layer;
static GBitmap *weather_bitmap = NULL;

int hero = 3;
int current_frame, starting_frame;
int ending_frame;
int delay;//delay between each frame is in milliseconds


#define TOTAL_LIFE 11
static GBitmap *life_images[TOTAL_LIFE];
static BitmapLayer *life_layers[TOTAL_LIFE];

static GBitmap *step_images[TOTAL_LIFE];
static BitmapLayer *step_layers[TOTAL_LIFE];

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

#define TOTAL_HERO 2
static GBitmap *hero_images[TOTAL_HERO];
static BitmapLayer *hero_layers[TOTAL_HERO];

const int animation_frames[] = {  
RESOURCE_ID_Eli00,
RESOURCE_ID_Eli01,
RESOURCE_ID_Eli02,
RESOURCE_ID_Eli03,
RESOURCE_ID_Eli04,
RESOURCE_ID_Eli05,
RESOURCE_ID_Eli06,
RESOURCE_ID_Eli07,
RESOURCE_ID_Eli08,
RESOURCE_ID_Eli09,
RESOURCE_ID_Eli10,
RESOURCE_ID_Eli11,
RESOURCE_ID_Eli12,
RESOURCE_ID_Eli13,
RESOURCE_ID_Eli14,
RESOURCE_ID_Eli15,
RESOURCE_ID_Eli16,
RESOURCE_ID_Eli17,
RESOURCE_ID_Eli18,
RESOURCE_ID_Eli19,
RESOURCE_ID_Eli20,
RESOURCE_ID_Eli21,
RESOURCE_ID_Eli22,
RESOURCE_ID_Eli23,
RESOURCE_ID_Eli24,
RESOURCE_ID_Eli25,
RESOURCE_ID_Eli26,
RESOURCE_ID_Eli27,
RESOURCE_ID_Eli28,
RESOURCE_ID_Eli29,
RESOURCE_ID_Eli30,
RESOURCE_ID_Eli31,
RESOURCE_ID_Eli32,
RESOURCE_ID_Eli33,
RESOURCE_ID_Eli34,
RESOURCE_ID_Eli35,
RESOURCE_ID_Eli36,
RESOURCE_ID_Eli37,
RESOURCE_ID_Eli38,
RESOURCE_ID_Eli39,
RESOURCE_ID_Eli40,
RESOURCE_ID_Eli41,
RESOURCE_ID_Eli42,
RESOURCE_ID_Eli43,
RESOURCE_ID_Eli44,
RESOURCE_ID_Eli45,
RESOURCE_ID_Eli46,
RESOURCE_ID_Eli47,
RESOURCE_ID_Eli48,
RESOURCE_ID_Eli49,
RESOURCE_ID_Eli50,
RESOURCE_ID_Eli51,
RESOURCE_ID_Eli52,
RESOURCE_ID_Eli53,
RESOURCE_ID_Eli54,
RESOURCE_ID_Eli55,
RESOURCE_ID_Eli56,
RESOURCE_ID_Eli57,
RESOURCE_ID_Eli58,
RESOURCE_ID_Eli59,
RESOURCE_ID_Eli60,
RESOURCE_ID_Eli61,
RESOURCE_ID_Eli62,
RESOURCE_ID_Eli63,
RESOURCE_ID_Eli64,
RESOURCE_ID_Eli65,
RESOURCE_ID_Eli66,
RESOURCE_ID_Eli67,
RESOURCE_ID_Eli68,
RESOURCE_ID_Eli69,
RESOURCE_ID_Eli70,
RESOURCE_ID_Eli71,
RESOURCE_ID_Eli72,
RESOURCE_ID_Eli73,
RESOURCE_ID_Eli74,
RESOURCE_ID_Eli75, //76

RESOURCE_ID_LYN00, //77
RESOURCE_ID_LYN01,
RESOURCE_ID_LYN02,
RESOURCE_ID_LYN03,
RESOURCE_ID_LYN04,
RESOURCE_ID_LYN05,
RESOURCE_ID_LYN06,
RESOURCE_ID_LYN07,
RESOURCE_ID_LYN08,
RESOURCE_ID_LYN09,
RESOURCE_ID_LYN10,
RESOURCE_ID_LYN11,
RESOURCE_ID_LYN12,
RESOURCE_ID_LYN13,
RESOURCE_ID_LYN14,
RESOURCE_ID_LYN15,
RESOURCE_ID_LYN16,
RESOURCE_ID_LYN17,
RESOURCE_ID_LYN18,
RESOURCE_ID_LYN19,
RESOURCE_ID_LYN20,
RESOURCE_ID_LYN21,
RESOURCE_ID_LYN22,
RESOURCE_ID_LYN23,
RESOURCE_ID_LYN24,
RESOURCE_ID_LYN25,
RESOURCE_ID_LYN26,
RESOURCE_ID_LYN27,
RESOURCE_ID_LYN28,
RESOURCE_ID_LYN29,
RESOURCE_ID_LYN30,
RESOURCE_ID_LYN31,
RESOURCE_ID_LYN32,
RESOURCE_ID_LYN33,
RESOURCE_ID_LYN34,
RESOURCE_ID_LYN35,
RESOURCE_ID_LYN36,
RESOURCE_ID_LYN37,
RESOURCE_ID_LYN38,
RESOURCE_ID_LYN39,
RESOURCE_ID_LYN40,
RESOURCE_ID_LYN41,
RESOURCE_ID_LYN42,
RESOURCE_ID_LYN43,
RESOURCE_ID_LYN44,
RESOURCE_ID_LYN45,
RESOURCE_ID_LYN46,
RESOURCE_ID_LYN47,
RESOURCE_ID_LYN48,
RESOURCE_ID_LYN49,
RESOURCE_ID_LYN50,
RESOURCE_ID_LYN51,
RESOURCE_ID_LYN52,
RESOURCE_ID_LYN53,
RESOURCE_ID_LYN54,
RESOURCE_ID_LYN55,
RESOURCE_ID_LYN56,
RESOURCE_ID_LYN57,
RESOURCE_ID_LYN58,
RESOURCE_ID_LYN59,
RESOURCE_ID_LYN60,
RESOURCE_ID_LYN61,
RESOURCE_ID_LYN62,
RESOURCE_ID_LYN63,
RESOURCE_ID_LYN64,
RESOURCE_ID_LYN65, //142
  
RESOURCE_ID_Hector00, //143
RESOURCE_ID_Hector01,
RESOURCE_ID_Hector02,
RESOURCE_ID_Hector03,
RESOURCE_ID_Hector04,
RESOURCE_ID_Hector05,
RESOURCE_ID_Hector06,
RESOURCE_ID_Hector07,
RESOURCE_ID_Hector08,
RESOURCE_ID_Hector09,
RESOURCE_ID_Hector10,
RESOURCE_ID_Hector11,
RESOURCE_ID_Hector12,
RESOURCE_ID_Hector13,
RESOURCE_ID_Hector14,
RESOURCE_ID_Hector15,
RESOURCE_ID_Hector16,
RESOURCE_ID_Hector17,
RESOURCE_ID_Hector18,
RESOURCE_ID_Hector19,
RESOURCE_ID_Hector20,
RESOURCE_ID_Hector21,
RESOURCE_ID_Hector22,
RESOURCE_ID_Hector23,
RESOURCE_ID_Hector24,
RESOURCE_ID_Hector25,
RESOURCE_ID_Hector26,
RESOURCE_ID_Hector27,
RESOURCE_ID_Hector28,
RESOURCE_ID_Hector29,
RESOURCE_ID_Hector30,
RESOURCE_ID_Hector31,
RESOURCE_ID_Hector32,
RESOURCE_ID_Hector33,
RESOURCE_ID_Hector34,
RESOURCE_ID_Hector35,
RESOURCE_ID_Hector36,
RESOURCE_ID_Hector37,
RESOURCE_ID_Hector38,
RESOURCE_ID_Hector39,//182
};


static void timer_handler(void *context) 
{
  if(current_frame < ending_frame){
    if(current_frame == ending_frame){
     current_frame = starting_frame;
    }
    if (hero_images[1] != NULL) {
      gbitmap_destroy(hero_images[1]);
      hero_images[1] = NULL;
    }
    
    hero_images[1] = gbitmap_create_with_resource(animation_frames[current_frame]);
    
    bitmap_layer_set_bitmap(hero_layers[1], hero_images[1]);
    layer_mark_dirty(bitmap_layer_get_layer(hero_layers[1]));
    
    if(current_frame == 43){vibes_short_pulse();}
    else if(current_frame == 118){vibes_short_pulse();}
    else if(current_frame == 174){vibes_short_pulse();}

    current_frame++;
    app_timer_register(delay, timer_handler, NULL);
  }
}

static void load_sequence() 
{
  
  if(hero == 1){
  current_frame = 0;
  ending_frame = 76;
  delay = 100;
  }
  else if(hero == 2){
  current_frame = 77;
  ending_frame = 142;
  delay = 100;
  }
  else if(hero == 3){
  current_frame = 143;
  ending_frame = 182;
  delay = 100; 
  }
  starting_frame = current_frame;
  app_timer_register(1, timer_handler, NULL);
}

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) 
{
GBitmap *old_image = *bmp_image;

 	*bmp_image = gbitmap_create_with_resource(resource_id);
 	GRect frame = (GRect) {
   	.origin = origin,
    .size = gbitmap_get_bounds(*bmp_image).size 
};
 	bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
 	layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

 	if (old_image != NULL) {
 	gbitmap_destroy(old_image);
  }
}

static void load_hero_layer()
{ 
       if(hero == 1) { set_container_image(&hero_images[1], hero_layers[1], animation_frames[0], GPoint(0,0) );}
  else if(hero == 2) { set_container_image(&hero_images[1], hero_layers[1], animation_frames[77], GPoint(0,0) );}
  else if(hero == 3) { set_container_image(&hero_images[1], hero_layers[1], animation_frames[143], GPoint(0,0) );}
}

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
  
  layer_mark_dirty(step_bar);
		
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
   if(step_percent < 10) {set_container_image(&step_images[0], step_layers[0], LIFE_ICONS[0], GPoint(34, 149));}
   else if(step_percent >= 10 && step_percent < 20) {set_container_image(&step_images[0], step_layers[0], LIFE_ICONS[1], GPoint(34, 149));}
   else if(step_percent >= 20 && step_percent < 30) {set_container_image(&step_images[0], step_layers[0], LIFE_ICONS[2], GPoint(34, 149));} 
   else if(step_percent >= 30 && step_percent < 40) {set_container_image(&step_images[0], step_layers[0], LIFE_ICONS[3], GPoint(34, 149));}
   else if(step_percent >= 40 && step_percent < 50) {set_container_image(&step_images[0], step_layers[0], LIFE_ICONS[4], GPoint(34, 149));}
   else if(step_percent >= 50 && step_percent < 60) {set_container_image(&step_images[0], step_layers[0], LIFE_ICONS[5], GPoint(34, 149));}
   else if(step_percent >= 60 && step_percent < 70) {set_container_image(&step_images[0], step_layers[0], LIFE_ICONS[6], GPoint(34, 149));}
   else if(step_percent >= 70 && step_percent < 80) {set_container_image(&step_images[0], step_layers[0], LIFE_ICONS[7], GPoint(34, 149));}
   else if(step_percent >= 80 && step_percent < 90) {set_container_image(&step_images[0], step_layers[0], LIFE_ICONS[8], GPoint(34, 149));}
   else if(step_percent >= 90 && step_percent < 100) {set_container_image(&step_images[0], step_layers[0], LIFE_ICONS[9], GPoint(34, 149));}
   else if(step_percent >= 100) {set_container_image(&step_images[0], step_layers[0], LIFE_ICONS[10], GPoint(34, 149));}
}

static void battery_callback(BatteryChargeState state) {
  battery_level = state.charge_percent;
  layer_mark_dirty(battery_layer);
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  
  static char battery_buffer[32];
  snprintf(battery_buffer, sizeof(battery_buffer), "%d", battery_level);
  text_layer_set_text(text_battery_layer, battery_buffer);
  
  if(battery_level < 10) {set_container_image(&life_images[0], life_layers[0], LIFE_ICONS[0], GPoint(106, 149));}
   else if(battery_level >= 10 && battery_level < 20) {set_container_image(&life_images[0], life_layers[0], LIFE_ICONS[1], GPoint(106, 149));}
   else if(battery_level >= 20 && battery_level < 30) {set_container_image(&life_images[0], life_layers[0], LIFE_ICONS[2], GPoint(106, 149));} 
   else if(battery_level >= 30 && battery_level < 40) {set_container_image(&life_images[0], life_layers[0], LIFE_ICONS[3], GPoint(106, 149));}
   else if(battery_level >= 40 && battery_level < 50) {set_container_image(&life_images[0], life_layers[0], LIFE_ICONS[4], GPoint(106, 149));}
   else if(battery_level >= 50 && battery_level < 60) {set_container_image(&life_images[0], life_layers[0], LIFE_ICONS[5], GPoint(106, 149));}
   else if(battery_level >= 60 && battery_level < 70) {set_container_image(&life_images[0], life_layers[0], LIFE_ICONS[6], GPoint(106, 149));}
   else if(battery_level >= 70 && battery_level < 80) {set_container_image(&life_images[0], life_layers[0], LIFE_ICONS[7], GPoint(106, 149));}
   else if(battery_level >= 80 && battery_level < 90) {set_container_image(&life_images[0], life_layers[0], LIFE_ICONS[8], GPoint(106, 149));}
   else if(battery_level >= 90 && battery_level < 100) {set_container_image(&life_images[0], life_layers[0], LIFE_ICONS[9], GPoint(106, 149));}
   else if(battery_level == 100) {set_container_image(&life_images[0], life_layers[0], LIFE_ICONS[10], GPoint(106, 149));}
  
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //Background
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_Night);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 122));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  //Enemy
  enemy_bitmap = gbitmap_create_with_resource(RESOURCE_ID_Enemy);
  enemy_layer = bitmap_layer_create(GRect(30,81, 36, 33));
  bitmap_layer_set_bitmap(enemy_layer, enemy_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(enemy_layer));
  bitmap_layer_set_compositing_mode(enemy_layer, GCompOpSet);
  

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 25), bounds.size.w, 50));
  o_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 25), bounds.size.w, 50));
  month_layer = text_layer_create(GRect(0,1,54,30));
  day_layer = text_layer_create(GRect(92,2,54,25));
  temperature_layer = text_layer_create(GRect(104,112,40,25));
  step_layer = text_layer_create(GRect(-3,138,40,25));
  text_battery_layer = text_layer_create(GRect(69,138,40,25));
  weather_layer = bitmap_layer_create(GRect(2, 117, 37, 23));
  
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
  
  //Battery Bar
  battery_layer = layer_create(GRect(106, 149, 33, 11));
  layer_set_update_proc(battery_layer, battery_update_proc);
  
  //Step Bar
  step_bar = layer_create(GRect(34, 149, 33, 11));
  layer_set_update_proc(step_bar, &step_layer_update_callback);
  
  //Battery Text
  text_layer_set_background_color(text_battery_layer, GColorClear);
  text_layer_set_text_color(text_battery_layer, GColorWhite);
  text_layer_set_text_alignment(text_battery_layer, GTextAlignmentCenter);
 
  //Step Text
  text_layer_set_background_color(step_layer, GColorClear);
  text_layer_set_text_color(step_layer, GColorWhite);
  text_layer_set_text_alignment(step_layer, GTextAlignmentCenter);

 
  GRect dummy_frame = { {0, 0}, {0, 0} };
  
  for (int i = 0; i < TOTAL_HERO; ++i) {
   	hero_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(hero_layers[i]));
    bitmap_layer_set_compositing_mode(hero_layers[i], GCompOpSet);
  }
   
  load_sequence();
  load_hero_layer();
  
  //UI
  ui_bitmap = gbitmap_create_with_resource(RESOURCE_ID_UI);
  ui_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(ui_layer, ui_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(ui_layer));
  bitmap_layer_set_compositing_mode(ui_layer, GCompOpSet);
  
  //Weapon
  weapon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_Weapons1);
  weapon_layer = bitmap_layer_create(GRect(41, 123, 60, 16));
  bitmap_layer_set_bitmap(weapon_layer, weapon_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(weapon_layer));
  bitmap_layer_set_compositing_mode(weapon_layer, GCompOpSet);

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

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(o_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(month_layer));
  layer_add_child(window_layer, text_layer_get_layer(day_layer));
  layer_add_child(window_layer, text_layer_get_layer(temperature_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(weather_layer));
  layer_add_child(window_layer, battery_layer);
  layer_add_child(window_layer, step_bar);
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
  bitmap_layer_destroy(ui_layer);
  bitmap_layer_destroy(enemy_layer);
  bitmap_layer_destroy(weapon_layer);
  
  for (int i = 0; i < TOTAL_HERO; i++) {
   	layer_remove_from_parent(bitmap_layer_get_layer(hero_layers[i]));
   	gbitmap_destroy(hero_images[i]);
   	bitmap_layer_destroy(hero_layers[i]); 
  }
  
  for (int i = 0; i < TOTAL_LIFE; i++) {
   	layer_remove_from_parent(bitmap_layer_get_layer(life_layers[i]));
   	gbitmap_destroy(life_images[i]);
   	bitmap_layer_destroy(life_layers[i]); 
  }
  
   for (int i = 0; i < TOTAL_LIFE; i++) {
   	layer_remove_from_parent(bitmap_layer_get_layer(step_layers[i]));
   	gbitmap_destroy(step_images[i]);
   	bitmap_layer_destroy(step_layers[i]); 
  }
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  Layer *window_layer = window_get_root_layer(s_main_window);

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
  
  GRect dummy_frame = { {0, 0}, {0, 0} };

  for (int i = 0; i < TOTAL_LIFE; ++i) {
   	life_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_layer, bitmap_layer_get_layer(life_layers[i]));
  }
  
  for (int i = 0; i < TOTAL_LIFE; ++i) {
   	step_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_layer, bitmap_layer_get_layer(step_layers[i]));
  }
  
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
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}