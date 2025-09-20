-# USING NEW MAKEFILE v3
# USING NEW MAKEFILE v4 (SDL2 touch demo)
 CXX      := g++
 CXXFLAGS := -O2 -std=gnu++17 -DNDEBUG -Iinclude
-LDFLAGS  :=
LDFLAGS  :=
 
 SRCS := \
   src/demo/hello.cpp \
   src/main.cpp
 
 OBJS := $(SRCS:.cpp=.o)
 
 .build/program: $(OBJS)
-	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)
	@pkg-config --exists sdl2 || sdl2-config --version >/dev/null 2>&1 || { \
	  echo "ERROR: SDL2 dev not found. Install with:" ; \
	  echo "  sudo apt update && sudo apt install -y libsdl2-dev" ; \
	  exit 1 ; }
	$(CXX) $(CXXFLAGS) `sdl2-config --cflags` -o $@ $(OBJS) $(LDFLAGS) `sdl2-config --libs`
 
 %.o: %.cpp
-	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(CXX) $(CXXFLAGS) `sdl2-config --cflags` -c $< -o $@
 
 .PHONY: clean
 clean:
 	rm -f $(OBJS) .build/program
-// DEST: src/main.cpp
-#include <iostream>
-#include "demo/hello.hpp"
-
-int main() {
-    std::cout << "=== Waterbutt Controller Demo v24 (patch) ===" << std::endl;
-    for (int i = 3; i >= 1; --i) {
-        std::cout << "count " << i << std::endl;
-    }
-    hello();
-    std::cout << "=== Finished v24 ===" << std::endl;
-    return 0;
-}
// DEST: src/main.cpp
// SDL2 fullscreen “hello-touch”: draws small red squares where you touch.
// Touch is read from an evdev device (default /dev/input/event5, override via env TOUCH_DEV).
// Esc or Ctrl+C quits.

#include <SDL2/SDL.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <atomic>
#include <thread>
#include <vector>
#include <cstring>
#include <iostream>
#include <optional>
#include "demo/hello.hpp"

struct AbsInfo { int min=0, max=4095; };
struct TouchState {
  std::atomic<int> x{640};
  std::atomic<int> y{400};
  std::atomic<bool> have{false};
  std::atomic<bool> running{true};
  AbsInfo ax, ay;
};

static std::optional<AbsInfo> get_abs(int fd, int code) {
  input_absinfo ai{};
  if (ioctl(fd, EVIOCGABS(code), &ai) == 0) {
    AbsInfo r; r.min = ai.minimum; r.max = ai.maximum;
    if (r.max <= r.min) { r.min = 0; r.max = 4095; }
    return r;
  }
  return std::nullopt;
}

static void touch_thread(const char* dev, TouchState* st) {
  int fd = open(dev, O_RDONLY | O_NONBLOCK);
  if (fd < 0) {
    std::perror(("open evdev " + std::string(dev)).c_str());
    st->running = false;
    return;
  }
  // Try to fetch real ranges
  if (auto xai = get_abs(fd, ABS_X)) st->ax = *xai;
  if (auto yai = get_abs(fd, ABS_Y)) st->ay = *yai;

  // Drain events
  while (st->running.load()) {
    input_event ev;
    ssize_t n = read(fd, &ev, sizeof(ev));
    if (n == sizeof(ev)) {
      if (ev.type == EV_ABS) {
        if (ev.code == ABS_X) { st->x = ev.value; }
        else if (ev.code == ABS_Y) { st->y = ev.value; }
      } else if (ev.type == EV_KEY && ev.code == BTN_TOUCH) {
        st->have = (ev.value != 0);
      }
    } else {
      // No data right now
      usleep(1000);
    }
  }
  close(fd);
}

static int map_coord(int v, const AbsInfo& a, int outMax) {
  if (v < a.min) v = a.min;
  if (v > a.max) v = a.max;
  const int inSpan = a.max - a.min;
  if (inSpan <= 0) return 0;
  return (int)((long long)(v - a.min) * (outMax - 1) / inSpan);
}

int main(int, char**) {
  std::cout << "=== Waterbutt Touch Demo (SDL2) ===\n";
  hello();

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
    return 1;
  }

  // Fullscreen window on the primary display
  SDL_DisplayMode dm; SDL_GetCurrentDisplayMode(0, &dm);
  int W = dm.w > 0 ? dm.w : 1280;
  int H = dm.h > 0 ? dm.h : 800;

  SDL_Window* win = SDL_CreateWindow(
      "Touch Demo",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      W, H,
      SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI
  );
  if (!win) {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!ren) {
    std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  // Touch reader thread
  const char* dev = std::getenv("TOUCH_DEV");
  if (!dev || !*dev) dev = "/dev/input/event5";
  TouchState ts;
  std::thread thr(touch_thread, dev, &ts);

  std::cout << "Using touch device: " << dev << "\n";
  std::cout << "Abs ranges: X[" << ts.ax.min << "," << ts.ax.max << "]  Y[" << ts.ay.min << "," << ts.ay.max << "]\n";
  std::cout << "Press ESC or Ctrl+C in terminal to quit.\n";

  struct Dot { int x,y,life; };
  std::vector<Dot> dots;
  dots.reserve(1024);

  bool quit = false;
  uint32_t lastSpawn = SDL_GetTicks();

  while (!quit && ts.running.load()) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) quit = true;
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) quit = true;
    }

    // Add a dot when we have touch (throttle spawns)
    if (ts.have.load()) {
      uint32_t now = SDL_GetTicks();
      if (now - lastSpawn > 16) {
        int px = map_coord(ts.x.load(), ts.ax, W);
        int py = map_coord(ts.y.load(), ts.ay, H);
        dots.push_back({px, py, 255});
        if (dots.size() > 2000) dots.erase(dots.begin(), dots.begin()+500);
        lastSpawn = now;
      }
    }
    // Fade / cull
    for (auto& d : dots) if (d.life > 0) d.life -= 4;
    dots.erase(std::remove_if(dots.begin(), dots.end(), [](const Dot& d){ return d.life <= 0; }), dots.end());

    // Draw
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    for (const auto& d : dots) {
      // simple red square with alpha based on life
      Uint8 a = (Uint8)std::max(0, std::min(255, d.life));
      SDL_SetRenderDrawColor(ren, 255, 0, 0, a);
      SDL_Rect r{ d.x-4, d.y-4, 9, 9 };
      SDL_RenderFillRect(ren, &r);
    }

    // HUD text substitute: a border + crosshair (no TTF to keep deps minimal)
    SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
    // border
    SDL_Rect b{0,0,W,H};
    SDL_RenderDrawRect(ren, &b);
    // crosshair at current mapped touch
    int cx = map_coord(ts.x.load(), ts.ax, W);
    int cy = map_coord(ts.y.load(), ts.ay, H);
    SDL_RenderDrawLine(ren, cx-12, cy, cx+12, cy);
    SDL_RenderDrawLine(ren, cx, cy-12, cx, cy+12);

    SDL_RenderPresent(ren);
  }

  ts.running = false;
  if (thr.joinable()) thr.join();
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  std::cout << "=== Finished touch demo ===\n";
  return 0;
}