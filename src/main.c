#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define TOOL_PENCIL 0
#define TOOL_RECT   1

#define COLOR_BLACK 0xFF000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_RED   0xFFFF0000
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLUE  0xFF0000FF

/* probably going to want to add stuff before exit later */
void mage_exit(int code)
{
	exit(code);
}

void mage_error(const char* str)
{
	printf("ERROR: %s", str);
	mage_exit(1);
}

SDL_Surface* mage_load_image(char* f)
{
	SDL_Surface* tmp;
	SDL_Surface* surface;

	printf("Loading image %s.\n", f);
	
	tmp = IMG_Load(f);
	if (tmp == NULL) {
		mage_error("Failed to load image.");
	}

	surface = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_BGRA32, NULL);
	if (surface == NULL) {
		mage_error("Failed to convert image to BGRA32.");
	}

	SDL_FreeSurface(tmp);
	return surface;
}

void mage_pencil(SDL_Surface* img, uint32_t color, int x, int y)
{
	/* warning: endianness is ABGR */
	uint32_t* pixels = img->pixels;
	if (SDL_MUSTLOCK(img)) {
		SDL_LockSurface(img);
	}
	/* adjust for status bar (hack!) */
	y -= 16;

	pixels[(y*img->w)+x] = color;
	if (SDL_MUSTLOCK(img)) {
		SDL_UnlockSurface(img);
	}
}

void mage_rect(SDL_Surface* img, uint32_t color, int x, int y, int w, int h)
{
	/* warning: endianness is ABGR */
	uint32_t* pixels = img->pixels;
	SDL_Rect rect;
	if (SDL_MUSTLOCK(img)) {
		SDL_LockSurface(img);
	}
	/* adjust for status bar (hack!) */
	y -= 16;

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_FillRect(img, &rect, color);
	/*for (int i = x; i < x+w; ++i) {
		for (int ii = y; ii < y+h; ++ii) {
			pixels[(ii*img->w)+i] = color;
		}
	}*/
	if (SDL_MUSTLOCK(img)) {
		SDL_UnlockSurface(img);
	}
}

void mage_render
(SDL_Window* win, SDL_Surface* screen, SDL_Surface* img, SDL_Surface* overlay, uint32_t color)
{
	/* screen clear and status bar */
	SDL_Rect rect;
	SDL_FillRect(screen, NULL, COLOR_BLACK);
	rect.w = 16;
	rect.h = 16;
	rect.x = 0;
	rect.y = 0;
	SDL_FillRect(screen, &rect, color);
	rect.y = -16;

	rect.w = img->w;
	rect.h = img->h;
	SDL_BlitSurface(img, &rect, screen, NULL);
	rect.w = overlay->w;
	rect.h = overlay->h;
	SDL_BlitSurface(overlay, &rect, screen, NULL);
	SDL_UpdateWindowSurface(win);
}

int main(int argc, char* argv[])
{
	SDL_Window* win;
	SDL_Surface* screen;
	SDL_Surface* img;
	SDL_Surface* overlay;
	uint32_t color = COLOR_BLACK;
	int tool = TOOL_PENCIL;
	int mouse_down = 0;
	int win_width = 640;
	int win_height = 480;
	int x = 0;
	int y = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		mage_error("Failed to initialize SDL!");
	}

	win = SDL_CreateWindow("mage", 0, 0, win_width, win_height, SDL_WINDOW_SHOWN);
	if (win == NULL) {
		mage_error("Failed to initialize SDL window!");
	}

	screen = SDL_GetWindowSurface(win);

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		mage_error("Failed to initialize SDL_image!");
	}

	if (argc > 1) {
		img = mage_load_image(argv[1]);
	} else {
		/* fall back to a blank canvas */
		img = SDL_CreateRGBSurfaceWithFormat(NULL,
											 win_width,
											 win_height,
											 8,
											 SDL_PIXELFORMAT_BGRA32);
		SDL_FillRect(img, NULL, COLOR_WHITE);
	}
	if (img == NULL) {
		mage_error("Failed to initialize image surface!");
	}

	overlay = SDL_CreateRGBSurfaceWithFormat(NULL,
											 img->w,
											 img->h,
											 8,
											 SDL_PIXELFORMAT_BGRA32);
	if (overlay == NULL) {
		mage_error("Failed to initialize overlay surface!");
	}


	/* so we don't start with a black screen */
	mage_render(win, screen, img, overlay, color);
	while (1) {
		SDL_Event e;
		int should_draw = 0;

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				mage_exit(0);
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_q || e.key.keysym.sym == SDLK_ESCAPE) {
					mage_exit(0);
				}
				if (e.key.keysym.sym == SDLK_b) {
					tool = TOOL_PENCIL;
					should_draw = 1;
				}
				if (e.key.keysym.sym == SDLK_r) {
					tool = TOOL_RECT;
					should_draw = 1;
				}
				if (e.key.keysym.sym == SDLK_1) {
					color = COLOR_BLACK;
					should_draw = 1;
				}
				if (e.key.keysym.sym == SDLK_2) {
					color = COLOR_WHITE;
					should_draw = 1;
				}
				if (e.key.keysym.sym == SDLK_3) {
					color = COLOR_RED;
					should_draw = 1;
				}
				if (e.key.keysym.sym == SDLK_4) {
					color = COLOR_GREEN;
					should_draw = 1;
				}
				if (e.key.keysym.sym == SDLK_5) {
					color = COLOR_BLUE;
					should_draw = 1;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (e.button.button == SDL_BUTTON_LEFT) {
					switch (tool) {
					case TOOL_PENCIL:
						mage_pencil(img, color, e.button.x, e.button.y);
						break;
					case TOOL_RECT:
						x = e.button.x;
						y = e.button.y;
						break;
					}
					mouse_down = 1;
					should_draw = 1;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (mouse_down && e.button.button == SDL_BUTTON_LEFT) {
					switch (tool) {
					case TOOL_RECT:
						mage_rect(img, color,
								  x<e.button.x?x:e.button.x,
								  y<e.button.y?y:e.button.y,
								  x<e.button.x?e.button.x-x:x-e.button.x,
								  y<e.button.y?e.button.y-y:y-e.button.y);
						break;
					}
					should_draw = 1;
				}
				mouse_down = 0;
				should_draw = 1;
				break;
			case SDL_MOUSEMOTION:
				if (mouse_down && e.button.button == SDL_BUTTON_LEFT) {
					switch (tool) {
					case TOOL_PENCIL:
						mage_pencil(img, color, e.button.x, e.button.y);
						break;
					case TOOL_RECT: {
						/* adjust for status bar (hack!) */
						int motion_x = e.motion.x;
						int motion_y = e.motion.y-16;
						int xx = x;
						int yy = y-16;
						SDL_Rect rect;
						rect.x = xx<motion_x?xx:motion_x;
						rect.y = yy<motion_y?yy:motion_y;
						rect.w = xx<motion_x?motion_x-xx:xx-motion_x;
						rect.h = yy<motion_y?motion_y-yy:yy-motion_y;
						SDL_FillRect(overlay, &rect, color);
						break;
					}
					}
					should_draw = 1;
				}
				break;
			default:
				break;
			}
		}
		if (should_draw) {
			mage_render(win, screen, img, overlay, color);
			SDL_FillRect(overlay, NULL, 0x00000000);
		}
	}
	return 0;
}
