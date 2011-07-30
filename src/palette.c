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

uint32_t picel_get_color(const PicelBitmap *map, int id) {
   // If index is valid, return the color
   if (id >= 0 && id <= 0xFF)
      return map->pal[id];

   // If index isn't valid, just return 0
   else
      return 0;
}

//***************************************************************************
// picel_set_color
// Changes a color inside a palette
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// param id: color index
// param color: new RGBA value
//***************************************************************************

void picel_set_color(PicelBitmap *map, int id, uint32_t color) {
   // Store color only if the ID is valid
   if (id >= 0 && id <= 0xFF)
      map->pal[id] = color;
}

