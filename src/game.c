#include <pebble.h>
#include <stdio.h>
#include "Ship.c"
  
Window* window;
Ship ship;
Bullet bullet;
Asteroid asteroid;
Layer* layer;
float speed;
int score = 0;
char showCredits = 0;
char gameState = 1;
static GBitmap* shipImage;
GPoint stars[100];
GColor colors[7];

static char isColliding(GRect rect, GPoint point) {
  if (point.x >= rect.origin.x && point.x <= rect.origin.x+rect.size.w
     && point.y >= rect.origin.y && point.y <= rect.origin.y+rect.size.h) {
    return 1;
  }
  return 0;
}

static char isCollidingRect(GRect rect, GRect rect2) {
  if (rect.origin.x > rect2.origin.x+rect2.size.w
     || rect.origin.x + rect.size.w < rect2.origin.x
     || rect.origin.y > rect2.origin.y+rect2.size.h
     || rect.origin.y + rect.size.h < rect2.origin.y) {
    return 0;
  }
  return 1;
}

static void timer_handler (void* context){
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating... %d", ship.x);
  layer_mark_dirty(layer);
  
  if (showCredits == 0) {
    if (gameState != 0) {
      //Set our bullet horribly off the screen
      bullet.y = -100;
      bullet.x = 0;
      
      speed = 1;
      
      //Create our asteroid
      asteroid.destroyed = 1;
      
      int i;
      for (i = 0; i < 100; i++) {
        stars[i].x = rand() % 144;
        stars[i].y = rand() % 159;
      }
    }
    
    ship.x+= ship.direction;
    if (ship.x > 134) { ship.x = 134; }
    if (ship.x < 0) { ship.x = 0;}
    
    //Respawn bullet if destroyed
    if (asteroid.destroyed > 0) {
      asteroid.destroyed = 0;
      asteroid.size = (rand() % 10)+5;
      asteroid.y = 0-(asteroid.size + 2);
      asteroid.x = (rand() % (144-(asteroid.size*2))) + asteroid.size;
    }
    
    //Destroy asteroid if off screen
    if (asteroid.y > 160) {
      asteroid.destroyed = 1;
      score--;
    }
    
    //Destroy bullet and asteroid if collide
    if (isColliding(GRect(asteroid.x-asteroid.size, asteroid.y-asteroid.size, asteroid.size*2, asteroid.size*2), GPoint(bullet.x,bullet.y))) {
      asteroid.destroyed = 1;
      bullet.y = -100;
      speed *= 1.05;
      score++;
    }
    
    if (isCollidingRect(GRect(asteroid.x-asteroid.size, asteroid.y-asteroid.size, asteroid.size*2, asteroid.size*2),GRect(ship.x,134, 10,10))) {
       gameState = 2;
    }
    
    bullet.y -= 10;
    asteroid.y += speed;
    int i;
    for (i = 0 ; i < 50 ; i++) { stars[i].y += 2; stars[i].y %= 159;}
    for (i = 50; i < 100; i++) { stars[i].y += 1; stars[i].x %= 159;}
  }
  
  app_timer_register(34, timer_handler, NULL);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  //If bullet is off screen, spawn it at ship+5
  if (bullet.y < 0) {
    bullet.y = 144;
    bullet.x = ship.x+5;
  }
  if (gameState != 0) {
      score = 0;
  }
  gameState = 0;
}

static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
  showCredits = (showCredits+1) % 2;
}

static void update(Layer* layer, GContext* ctx){
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx,GColorWhite);
  graphics_fill_rect(ctx, GRect(0,0,144,159), 0, GCornerNone);
  
  //graphics_draw_rect(ctx, GRect(ship.x,144,10,10));
  graphics_draw_bitmap_in_rect(ctx, shipImage, GRect(ship.x,139,11,13));
  
  //Make the bullet red
  graphics_context_set_stroke_color(ctx, colors[5]);
  graphics_draw_line(ctx, GPoint(bullet.x, bullet.y-10), GPoint(bullet.x, bullet.y));
  graphics_context_set_text_color(ctx, GColorWhite);
  
  
  char buf[15];
  
  
  graphics_context_set_text_color(ctx, colors[5]);
  if (showCredits == 1) {
    graphics_draw_text(ctx, "PAUSED!\nTilt Pebble to move ship.\nPress any button to shoot", fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(0, 25, 144, 159), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  } else {
    if (gameState == 0) {
      int i;
      for (i = 0; i < 100; i++) {
        graphics_context_set_stroke_color(ctx,colors[i%4]);
        graphics_draw_pixel(ctx, stars[i]);
      }
    }
    
    //Draw the asteroid after the pixels
    graphics_context_set_fill_color(ctx, colors[6]);
    graphics_fill_circle(ctx, GPoint(asteroid.x, asteroid.y), asteroid.size);
    
    if (gameState == 1) {
      graphics_draw_text(ctx, "ASTEROIDS", fonts_get_system_font(FONT_KEY_GOTHIC_24), GRect(0, 25, 144, 159), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      graphics_context_set_text_color(ctx, colors[1]);
      graphics_draw_text(ctx, "By: Mitch Davis\nand Sophia Farquhar", fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(0, 50, 144, 159), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      graphics_context_set_text_color(ctx, colors[2]);
      graphics_draw_text(ctx, "Tilt Pebble to move ship.\nPress any button to shoot\nPress any button to start", fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(0, 95, 144, 159), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
    
    if (gameState == 2) {
      graphics_draw_text(ctx, "Game Over\n\nPress any button to start", fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(0, 25, 144, 159), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
    
    graphics_context_set_text_color(ctx, colors[4]);
    snprintf(buf , 15, "Score: %d", score);
    graphics_draw_text(ctx, buf, fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(10, 10, 144, 159), GTextOverflowModeFill, GTextAlignmentLeft, NULL);
  }
}

static void data_handler(AccelData *data, uint32_t num_samples) {
  ship.direction = data[0].x / 50;
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
}

void handle_init(void) {
	// Create a window and text layer
	window = window_create();
  window_stack_push(window, true);
  ship.x = 0;
  ship.direction = 5;
  
  
  //Load up colors
  #ifdef PBL_COLOR
    colors[0] = GColorWhite;
    colors[1] = GColorMelon;
    colors[2] = GColorCeleste;
    colors[3] = GColorPastelYellow;
    colors[4] = GColorYellow;
    colors[5] = GColorRed;
    colors[6] = GColorWindsorTan;
  #else
    colors[0] = GColorWhite;
    colors[1] = GColorWhite;
    colors[2] = GColorWhite;
    colors[3] = GColorWhite;
    colors[4] = GColorWhite;
    colors[5] = GColorWhite;
    colors[6] = GColorWhite;
  #endif

  
  
  shipImage = gbitmap_create_with_resource(RESOURCE_ID_SHIP_IMAGE);
  
 layer = window_get_root_layer (window);
  layer_set_update_proc(layer, update);
  accel_data_service_subscribe(1, data_handler);
  window_set_click_config_provider(window, click_config_provider);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Triggering update!");
  app_timer_register(34, timer_handler, NULL);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "done update!");
}

void handle_deinit(void) {
	// Destroy the window
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}

