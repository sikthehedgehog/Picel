//***************************************************************************
// "palette.c"
// Palette manipulation
//***************************************************************************

// Required headers
#include <stdint.h>
#include "picel.h"
#include "bitmap.h"

//***************************************************************************
// picel_get_color
// Retrieves a color inside a palette
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// param id: color index
// return: RGBA value of color, or 0 on failure
//***************************************************************************

uint32_t picel_get_color(const PicelBitmap *map, uint8_t id) {
   return map->pal[id];
}

//***************************************************************************
// picel_set_color
// Changes a color inside a palette
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// param id: color index
// param color: new RGBA value
//***************************************************************************

void picel_set_color(PicelBitmap *map, uint8_t id, uint32_t color) {
   map->pal[id] = color;
}
