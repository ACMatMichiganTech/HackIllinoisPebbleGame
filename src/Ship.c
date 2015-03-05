#include <pebble.h>
  typedef struct{
    int x;
    int direction;
} Ship;

  typedef struct{
    int x;
    int y;
} Bullet;

  typedef struct{
    float x;
    float y;
    char destroyed;
    int size;
} Asteroid;

