//***************************************************************************
// "property.c"
// Bitmap properties
//***************************************************************************

// Required headers
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "picel.h"
#include "bitmap.h"

//***************************************************************************
// picel_get_bitmap_width
// Gets the width of a bitmap
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// return: width in pixels
//***************************************************************************

int picel_get_bitmap_width(const PicelBitmap *map) {
   return map->width;
}

//***************************************************************************
// picel_get_bitmap_height
// Gets the height of a bitmap
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// return: height in pixels
//***************************************************************************

int picel_get_bitmap_height(const PicelBitmap *map) {
   return map->height;
}

//***************************************************************************
// picel_is_bitmap_paletted
// Checks if a bitmap is using the palette
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// return: non-zero if using palette, zero otherwise
//***************************************************************************

int picel_is_bitmap_paletted(const PicelBitmap *map) {
   return map->paletted;
}

//***************************************************************************
// picel_convert_bitmap
// Converts a bitmap to a new size and/or format
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// param width: new width in pixels
// param height: new height in pixels
// param paletted: use a palette?
// return: zero on success or non-zero on failure
//***************************************************************************

int picel_convert_bitmap(PicelBitmap *map, int width, int height, int
paletted) {
   // If the properties are already the same, why bother?
   if (width == map->width && height == map->height &&
   paletted == map->paletted)
      return 0;

   // Create a new bitmap with the new properties. Sloppy, yes, but I don't
   // feel like duplicating the safety checks :P We'll copy the properties
   // that we need from the bitmap and then discard the new (temporal) bitmap
   PicelBitmap *temp = picel_create_bitmap(width, height, paletted);
   if (temp == NULL)
      return -1;

   // If the new bitmap is going to use a palette but the old bitmap didn't,
   // then copy the palette into the new bitmap, since we're going to need
   // that to ensure colors are converted properly.
   if (paletted && !map->paletted)
      memcpy(temp->pal, map->pal, sizeof(map->pal));

   // Determine which area to copy to the new bitmap
   int max_width = width < map->width ? width : map->width;
   int max_height = height < map->height ? height : map->height;

   // If both bitmaps share the same format (i.e. it's just a resizing), then
   // don't bother converting pixels and just copy pixels directly.
   if (paletted == map->paletted) {
      // Calculate the space taken up by a row
      size_t row_len = max_width * (paletted ? 1 : 4);

      // Copy all rows as-is to the new bitmap
      int y;
      for (y = 0; y < max_height; y++)
         memcpy(temp->rows[y], map->rows[y], row_len);
   }

   // If converting from paletted to true color, it's also relatively trivial
   // since we can just pick up the colors from the palette directly. Thereby
   // just convert it on our own too if that's the case.
   else if (!paletted) {
      // Scan all rows
      int y;
      for (y = 0; y < max_height; y++) {
         // Scan all pixels in this row
         uint8_t *src = map->rows[y];
         uint8_t *dest = temp->rows[y];

         int x;
         for (x = 0; x < max_width; x++) {
            // Retrieve RGBA color of this pixel from old bitmap
            uint32_t color = map->pal[*src];
            src++;

            // Store RGBA color of pixel into new bitmap
            *dest++ = color >> 24;
            *dest++ = color >> 16;
            *dest++ = color >> 8;
            *dest++ = color;
         }
      }
   }

   // Converting from true color to paletted is *not* trivial, since it
   // involves trying to find the nearest match in the palette for every
   // pixel. Since picel_set_pixel already does this, we're just gonna call
   // it... Yes, it's slow, but what did you expect?
   else {
      // Get pointer to first pixel in old bitmap. All pixels are stored
      // consecutively (no padding) so it's safe to just retrieve this
      // pointer once.
      uint8_t *ptr = map->rows[0];

      // Go through all pixels
      int x, y;
      for (y = 0; y < max_height; y++)
      for (x = 0; x < max_width; x++) {
         // Retrieve RGBA color of pixel
         uint32_t color = ptr[0] << 24 |
                          ptr[1] << 16 |
                          ptr[2] << 8 |
                          ptr[3];

         // Store this color in the new bitmap... Let the nearest-match
         // algorithm do its job
         picel_set_pixel(temp, x, y, color);

         // Go for next pixel
         ptr += 4;
      }
   }

   // OK, at this point we can't fail, so store the new properties into the
   // bitmap we want to convert
   map->width = width;
   map->height = height;
   map->paletted = paletted;

   // Get the new buffers
   free(map->data);
   free(map->rows);
   map->data = temp->data;
   map->rows = temp->rows;

   // Success!
   free(temp);
   return 0;
}

