#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

typedef struct {
  SDL_Rect rect;
  SDL_Color color;
  double xvel;
  double yvel;
  double xforce;
  double yforce;
} ball;

ball* balls;

int win_x;
int win_y;
int total_balls;

bool randomize_balls;

void spawn_ball(int x, int y) {
  //printf("color: %i\n", balls[0].color.r);
  total_balls++;
  printf("balls: %i\n", total_balls);
  balls = realloc(balls, sizeof(ball) * total_balls);

  if (x == NULL && y == NULL) {
    balls[total_balls - 1].rect.x = rand() % win_x;
    balls[total_balls - 1].rect.y = rand() % win_y;
  } else {
    balls[total_balls - 1].rect.x = x;
    balls[total_balls - 1].rect.y = y;
  }
  
  if (!randomize_balls) {
    balls[total_balls - 1].rect.w = 40;
    balls[total_balls - 1].rect.h = 40;
  } else {
    int size = (rand() % 70) + 10;
    balls[total_balls - 1].rect.w = size;
    balls[total_balls - 1].rect.h = size;
  }
  balls[total_balls - 1].xvel = rand() % 20;
  balls[total_balls - 1].yvel = 0;
  balls[total_balls - 1].yforce = 0;
  balls[total_balls - 1].color.r = rand() % 256;
  balls[total_balls - 1].color.g = rand() % 256;
  balls[total_balls - 1].color.b = rand() % 256;
}

int main(int argc, char** argv) {
  srand(time(NULL));
  SDL_Init(SDL_INIT_EVERYTHING);

  win_x = 900;
  win_y = 600;
  
  SDL_Window* win = SDL_CreateWindow("ApeiroEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_x, win_y, 0);
  SDL_SetWindowResizable(win, true);
  SDL_SetHint("SDL_HINT_RENDER_DRIVER", "opengl");
  SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  bool show_fps = false;
  bool show_colors = false;
  bool vsync_enabled = false;
  randomize_balls = false;
  
  TTF_Init();
  TTF_Font* retro = TTF_OpenFont("retro_font.ttf", 24);
  SDL_Color White = {255, 255, 255};
  SDL_Surface* text_surface = TTF_RenderText_Solid(retro, "fps", White);
  SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

  SDL_Rect text_rect;
  text_rect.x = 0;
  text_rect.y = 0;
  text_rect.w = 200;
  text_rect.h = 100;

  SDL_Surface* ball_surface = IMG_Load("ball.png");
  SDL_Texture* ball_texture = SDL_CreateTextureFromSurface(renderer, ball_surface);
  SDL_FreeSurface(ball_surface);

  total_balls = 1;
  balls = malloc(sizeof(ball));
  
  balls[0].rect.x = 900 / 2;
  balls[0].rect.y = 600 / 2;
  balls[0].rect.w = 40;
  balls[0].rect.h = 40;
  balls[0].xvel = 10;
  balls[0].yvel = 0;
  balls[0].yforce = 0;
  balls[0].color.r = 255;
  balls[0].color.g = 0;
  balls[0].color.b = 0;

  float g_force = -3.3;
  float elasticity = 0;
  float friction = 0;

  double target_fps = 60;

  Uint64 now = SDL_GetPerformanceCounter();
  Uint64 last = 0;
  double delta = 0;

  char fps_string[20];
  bool running = true;
  while (running) {
    now = SDL_GetPerformanceCounter();
    delta = (double)((now - last)*1000 / (double)SDL_GetPerformanceFrequency() );

    if (delta > (double) 1000 / target_fps) {
      last = now;

      SDL_GetWindowSize(win, &win_x, &win_y);
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
	if (event.type == SDL_QUIT) {
	  running = false;
	} else if (event.type == SDL_KEYDOWN) {
	  switch (event.key.keysym.scancode) {
	  case SDL_SCANCODE_LEFT:
	    target_fps -= 1;
	    break;
	  case SDL_SCANCODE_RIGHT:
	    target_fps += 1;
	    break;
	  case SDL_SCANCODE_DOWN:
	    elasticity += 0.5;
	    friction += 0.05;
	    break;
	  case SDL_SCANCODE_UP:
	    elasticity -= 0.5;
	    friction -= 0.05;
	    break;
	  case SDL_SCANCODE_F:
	    show_fps = !show_fps;
	    break;
	  case SDL_SCANCODE_SPACE:
	    spawn_ball(NULL, NULL);
	    break;
	  case SDL_SCANCODE_C:
	    show_colors = !show_colors;
	    break;
	  case SDL_SCANCODE_V:
	    if (vsync_enabled) {
	      SDL_GL_SetSwapInterval(0); 
	    } else {
	      SDL_GL_SetSwapInterval(1);
	    }
	    break;
	  case SDL_SCANCODE_Q:
	    total_balls = 1;
	    balls = realloc(balls, sizeof(ball));
	    break;
	  case SDL_SCANCODE_R:
	    randomize_balls = !randomize_balls;
	    break;
	  }
	}
      }

      int mouse_x, mouse_y = 0;
      if (SDL_GetMouseState(&mouse_x, &mouse_y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
	spawn_ball(mouse_x, mouse_y);
      }
      
      SDL_RenderClear(renderer);

      for (int i = 0; i < total_balls; i++) {
	if (show_colors) {
	  SDL_SetTextureColorMod(ball_texture, balls[i].color.r, balls[i].color.g, balls[i].color.b);
	} else {
	  SDL_SetTextureColorMod(ball_texture, 255, 255, 255);
	}
	
	SDL_RenderCopy(renderer, ball_texture, NULL, &balls[i].rect);
      }

      if (show_fps) {
        sprintf(fps_string, "%f fps", 1000.0f / delta );
	printf("target frametime: %f ms,", (double) 1000 / target_fps);
	printf("real frametime: %f ms, running at ", delta);
	puts(fps_string);
	text_surface = TTF_RenderText_Solid(retro, fps_string, White);
	text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
      }
      
      SDL_RenderPresent(renderer);

      for (int i = 0; i < total_balls; i++) {
        balls[i].yforce = 0;
        balls[i].yvel += balls[i].yforce + g_force;
        // ball_xvel = 10;
        //ball_yvel += (delta / 1000) * g_force;
        //ball_yvel = -1.0;
        balls[i].rect.y -= balls[i].yvel;
        balls[i].rect.x += balls[i].xvel;

        if (balls[i].rect.y + balls[i].rect.h > win_y) {
	  balls[i].yvel += elasticity;
	  balls[i].yvel *= -1;
	  balls[i].rect.y = win_y - balls[i].rect.h;
        } else if (balls[i].rect.y - balls[i].rect.h < 0) {
	  balls[i].yvel += elasticity;
	  balls[i].yvel *= -1;
	  balls[i].rect.y = balls[i].rect.h;
        }

        if (balls[i].rect.x + balls[i].rect.w > win_x) {
	  balls[i].xvel -= elasticity;
	  balls[i].xvel *= -1;
	  balls[i].rect.x = win_x - balls[i].rect.w;
        } else if (balls[i].rect.x < 0) {
	  balls[i].xvel += elasticity;
	  balls[i].xvel *= -1;
	  balls[i].rect.x = 0;
        }

	if (balls[i].rect.y >= win_y - balls[i].rect.h) {
	  if (balls[i].xvel - friction >= 0) {
	    balls[i].xvel -= friction;
	  } else if (balls[i].xvel + friction <= 0) {
	    balls[i].xvel += friction;
	  } else if (friction > 0) {
	    balls[i].xvel = 0;
	  }
	}
      }
      
    }
    
  }
  free(balls);
  SDL_DestroyTexture(ball_texture);
  SDL_DestroyTexture(text_texture);
  SDL_FreeSurface(text_surface);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
