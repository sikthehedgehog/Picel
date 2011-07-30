//***************************************************************************
// "drawing.c"
// Code for drawing primitives into a bitmap
//***************************************************************************

// Required headers
#include <stdint.h>
#include <stdlib.h>
#include "picel.h"
#include "bitmap.h"

//***************************************************************************
// picel_get_pixel
// Retrieves the RGBA color of a pixel in a bitmap
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// param x: horizontal coordinate
// param y: vertical coordinate
// return: RGBA color of pixel, or 0 on failure
//***************************************************************************

uint32_t picel_get_pixel(const PicelBitmap *map, int x, int y) {
   // Only retrieve color if inbounds...
   if (x >= 0 && y >= 0 && x < map->width && y < map->height) {
      // Non-paletted?
      if (!map->paletted) {
         const uint8_t *ptr = &map->rows[y][x << 2];
         return ptr[0] << 24 | ptr[1] << 16 | ptr[2] << 8 | ptr[3];
      }

      // Paletted?
      else
         return map->pal[map->rows[y][x]];
   }

   // Outbounds?
   else
      return 0;
}

//***************************************************************************
// picel_set_pixel
// Changes the RGBA color of a pixel in a bitmap
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// param x: horizontal coordinate
// param y: vertical coordinate
// param color: new RGBA color of pixel
//***************************************************************************

void picel_set_pixel(PicelBitmap *map, int x, int y, uint32_t color) {
   // Only set pixel if inbounds...
   if (x >= 0 && y >= 0 && x < map->width && y < map->height) {
      // Non-paletted?
      if (!map->paletted) {
         uint8_t *ptr = &map->rows[y][x << 2];
         ptr[0] = color >> 24;
         ptr[1] = color >> 16;
         ptr[2] = color >> 8;
         ptr[3] = color;
      }

      // Paletted?
      else {
         // Use this to keep track of the nearest match so far (set the
         // minimum difference to a high value by default so the first color
         // is always considered better)
         int mindiff = 99999;
         int best = 0;

         // Scan all colors x_X
         int i;
         for (i = 0; i < 0x100; i++) {
            // Huh, perfect match?
            uint32_t other = map->pal[i];
            if (color == other) {
               best = i;
               break;
            }

            // Nope, get each individual component
            uint8_t r1 = color >> 24; uint8_t r2 = other >> 24;
            uint8_t g1 = color >> 16; uint8_t g2 = other >> 16;
            uint8_t b1 = color >> 8;  uint8_t b2 = other >> 8;
            uint8_t a1 = color;       uint8_t a2 = other;

            // Determine difference between colors (using some quick "hack"
            // to give priority to those components that are more noticeable)
            int diff = abs(r1 - r2) + (abs(g1 - g2) << 1) +
                       abs(b1 - b2) + (abs(a1 - a2) << 1);

            // Is this a better match?
            if (diff < mindiff) {
               mindiff = diff;
               best = i;
            }
         }

         // Store best match into bitmap
         map->rows[y][x] = best;
      }
   }
}

//***************************************************************************
// picel_get_raw_pixel
// Retrieves the raw value of a pixel in a bitmap
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// param x: horizontal coordinate
// param y: vertical coordinate
// return: raw value of pixel, or 0 on failure
//***************************************************************************

uint32_t picel_get_raw_pixel(const PicelBitmap *map, int x, int y) {
   // Only retrieve color if inbounds...
   if (x >= 0 && y >= 0 && x < map->width && y < map->height) {
      // Paletted?
      if (map->paletted)
         return map->rows[y][x];

      // Non-paletted?
      else {
         const uint8_t *ptr = &map->rows[y][x << 2];
         return ptr[0] << 24 | ptr[1] << 16 | ptr[2] << 8 | ptr[3];
      }
   }

   // Outbounds?
   else
      return 0;
}

//***************************************************************************
// picel_set_raw_pixel
// Changes the raw value of a pixel in a bitmap
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// param x: horizontal coordinate
// param y: vertical coordinate
// param color: new raw value of pixel
//***************************************************************************

void picel_set_raw_pixel(PicelBitmap *map, int x, int y, uint32_t color) {
   // Only set pixel if inbounds...
   if (x >= 0 && y >= 0 && x < map->width && y < map->height) {
      // Paletted?
      if (map->paletted)
         map->rows[y][x] = color;

      // Non-paletted?
      else {
         uint8_t *ptr = &map->rows[y][x << 2];
         ptr[0] = color >> 24;
         ptr[1] = color >> 16;
         ptr[2] = color >> 8;
         ptr[3] = color;
      }
   }
}

//***************************************************************************
// picel_get_raw_data_ptr
// Returns a pointer to the raw pixel data
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// return: pointer to raw data
//***************************************************************************

uint8_t *picel_get_raw_data_ptr(PicelBitmap *map) {
   // Use it at your own risk...
   return map->data;
}
