//***************************************************************************
// "bitmap.h"
// Definition of the PicelBitmap structure
//***************************************************************************

#ifndef BITMAP_H
#define BITMAP_H

// Required headers
#include <stddef.h>
#include <stdint.h>
#include "picel.h"

// Contents of a bitmap
struct _PicelBitmap {
   size_t width;           // Width in pixels
   size_t height;          // Height in pixels
   int paletted;           // Uses palette?

   uint8_t *data;          // Raw bitmap data
   uint8_t **rows;         // Where each row starts
   uint32_t pal[0x100];    // Palette
};

#endif
