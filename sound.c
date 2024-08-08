#include "stdbool.h"
#include <mikmod.h>
#include <time.h>

bool initSound() {
  bool success = true;
  MikMod_RegisterAllDrivers();
  MikMod_RegisterAllLoaders();
  md_mode |= DMODE_SOFT_MUSIC;
  if (MikMod_Init("")) {
    fprintf(stderr, "Could not initialize sound, reason %s\n",
            MikMod_strerror(MikMod_errno));
    success = false;
  }
  return success;
}

void *playMusic(void *mod) {
  struct timespec req;
  req.tv_sec = 0;
  req.tv_nsec = 10000 * 1000; // 10000 microseconds = 10 milliseconds

  MODULE *module = Player_Load(mod, 64, 0);
  if (module) {
    Player_Start(module);
    while (Player_Active()) {
      nanosleep(&req, NULL);
      MikMod_Update();
    }
  }
}
