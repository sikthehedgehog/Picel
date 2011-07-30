//***************************************************************************
// "bitmap.c"
// Bitmap creation and such
//***************************************************************************

// Required headers
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "picel.h"
#include "bitmap.h"

//***************************************************************************
// picel_create_bitmap
// Creates a new bitmap object
//---------------------------------------------------------------------------
// param width: width in pixels
// param height: height in pixels
// param paletted: use a palette?
// return: pointer to bitmap or NULL on failure
//***************************************************************************

PicelBitmap *picel_create_bitmap(int width, int height, int paletted) {
   // Oh, c'mon?
   if (width <= 0 || height <= 0)
      return NULL;

   // Determine amount of memory needed to hold the bitmap data
   size_t size = paletted ? 1 : 4;
   if (SIZE_MAX / size <= width)
      return NULL;
   size *= width;
   size_t stride = size;
   if (SIZE_MAX / size <= height)
      return NULL;
   size *= height;

   // Determine amount of memory needed to hold the row list
   if (SIZE_MAX / sizeof(uint8_t*) <= height)
      return NULL;
   size_t rsize = sizeof(uint8_t*) * height;

   // Allocate memory for bitmap structure
   PicelBitmap *map = (PicelBitmap *) malloc(sizeof(PicelBitmap));
   if (map == NULL)
      return NULL;

   // Fill in bitmap properties
   map->width = width;
   map->height = height;
   map->paletted = paletted;

   // Allocate memory for bitmap data
   map->data = (uint8_t *) malloc(size);
   if (map->data == NULL) {
      free(map);
      return NULL;
   }

   // Allocate memory for row list
   map->rows = (uint8_t **) malloc(rsize);
   if (map->rows == NULL) {
      free(map->data);
      free(map);
      return NULL;
   }

   // Fill row list
   size_t i;
   uint8_t *rowptr = map->data;
   for (i = 0; i < height; i++) {
      map->rows[i] = rowptr;
      rowptr += stride;
   }

   // Success!
   return map;
}

//***************************************************************************
// picel_load_bitmap
// Loads a bitmap from a stream (detecting format automatically)
//---------------------------------------------------------------------------
// param name: name of stream
// return: pointer to bitmap or NULL on failure
//---------------------------------------------------------------------------
// TO-DO: add API functions that read off a stream directly without needing
// to open it on their own, so we don't need to open a stream twice here.
//***************************************************************************

PicelBitmap *picel_load_bitmap(const char *filename) {
   // Strings to check against to detect a file format by its header
   static const uint8_t png_header[] =
      { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

   // Attempt to open file and read its header
   uint8_t header[8];
   void *file = picel_get_open_func()(filename, 0);
   if (file == NULL)
      return NULL;
   if (picel_get_read_func()(file, header, 8)) {
      picel_get_close_func()(file);
      return NULL;
   }
   picel_get_close_func()(file);

   // Detect the format based on the header, and call the appropriate
   // function if we found a match
   if (memcmp(header, png_header, 8) == 0)
      return picel_load_png(filename);

   // Unknown format!
   return NULL;
}

//***************************************************************************
// picel_destroy_bitmap
// Deallocates a bitmap object
//---------------------------------------------------------------------------
// param map: pointer to bitmap
//***************************************************************************

void picel_destroy_bitmap(PicelBitmap *map) {
   // Just deallocate everything...
   free(map->data);
   free(map->rows);
   free(map);
}
