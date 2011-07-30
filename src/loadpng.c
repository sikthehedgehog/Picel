//***************************************************************************
// "loadpng.c"
// Code for loading a bitmap from a PNG stream
//***************************************************************************

// Required headers
#include <setjmp.h>
#include <png.h>
#include "picel.h"
#include "bitmap.h"

// Callback prototypes
static void func_read(png_structp, png_bytep, png_size_t);

//***************************************************************************
// picel_load_png
// Loads a bitmap from a PNG
//---------------------------------------------------------------------------
// param name: name of stream
// return: pointer to bitmap or NULL on failure
//***************************************************************************

PicelBitmap *picel_load_png(const char *name) {
   // Try to open stream
   void *stream = picel_get_open_func()(name, 0);
   if (stream == NULL)
      return NULL;

   // Create read structure
   png_structp png_ptr = png_create_read_struct
      (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      picel_get_close_func()(stream);
      return NULL;
   }

   // Create info structure
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL) {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      picel_get_close_func()(stream);
      return NULL;
   }

   // Setjmp shenanigans (ugh)
   if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      picel_get_close_func()(stream);
      return NULL;
   }

   // Set up I/O functions
   png_set_read_fn(png_ptr, stream, func_read);

   // We can handle maps this big, right?
   png_set_user_limits(png_ptr, 0x7FFFFFFF, 0x7FFFFFFF);

   // Read data into memory
   png_read_png(png_ptr, info_ptr,
      PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_SHIFT,
      NULL);

   // Get row pointers
   png_bytepp rows = png_get_rows(png_ptr, info_ptr);

   // Get bitmap properties
   int32_t width = png_get_image_width(png_ptr, info_ptr);
   int32_t height = png_get_image_height(png_ptr, info_ptr);
   int type = png_get_color_type(png_ptr, info_ptr);

   // Create a bitmap with the given properties
   PicelBitmap *map = picel_create_bitmap(width, height,
      type == PNG_COLOR_TYPE_PALETTE);
   if (map == NULL) {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      picel_get_close_func()(stream);
      return NULL;
   }

   // Read PNG data into bitmap
   int32_t x, y;
   uint8_t *dest = map->data;

   switch (type) {
      // Paletted?
      case PNG_COLOR_TYPE_PALETTE:
         // Go through all pixels and copy them as-is
         for (y = 0; y < height; y++) {
            const uint8_t *src = rows[y];
            for (x = 0; x < width; x++)
               *dest++ = *src++;
         }

         // We need to get the palette too!
         // Using scoping for the sake of making my life easier here
         // (these variables are only needed here and nowhere else)
         {
            // Get palette contents
            png_color *palette;
            int palsize;

            png_get_PLTE(png_ptr, info_ptr, &palette, &palsize);

            // Copy palettes into bitmap
            uint32_t *ptr = map->pal;
            for (x = 0; x < palsize; x++) {
               *ptr++ = palette[x].red   << 24 |
                        palette[x].green << 16 |
                        palette[x].blue  << 8 |
                        0xFF;
            }

            // Um, fill the missing entries, just in case...
            for (; x < 0x100; x++)
               *ptr++ = 0x00000000;
         }

         break;

      // Grayscale?
      case PNG_COLOR_TYPE_GRAY:
         // Go through all pixels and copy luminance into all components
         // Alpha is missing, so fill it with 0xFF (fully opaque)
         for (y = 0; y < height; y++) {
            const uint8_t *src = rows[y];
            for (x = 0; x < width; x++) {
               *dest++ = *src;
               *dest++ = *src;
               *dest++ = *src++;
               *dest++ = 0xFF;
            }
         }
         break;

      // Grayscale with alpha?
      case PNG_COLOR_TYPE_GRAY_ALPHA:
         // Go through all pixels and copy luminance into all components
         for (y = 0; y < height; y++) {
            const uint8_t *src = rows[y];
            for (x = 0; x < width; x++) {
               *dest++ = *src;
               *dest++ = *src;
               *dest++ = *src++;
               *dest++ = *src++;
            }
         }
         break;

      // RGB?
      case PNG_COLOR_TYPE_RGB:
         // Go through all pixels and copy them as-is
         // Alpha is missing, so fill it with 0xFF (fully opaque)
         for (y = 0; y < height; y++) {
            const uint8_t *src = rows[y];
            for (x = 0; x < width; x++) {
               *dest++ = *src++;
               *dest++ = *src++;
               *dest++ = *src++;
               *dest++ = 0xFF;
            }
         }
         break;

      // RGBA?
      case PNG_COLOR_TYPE_RGBA:
         // Go through all pixels and copy them as-is
         for (y = 0; y < height; y++) {
            const uint8_t *src = rows[y];
            for (x = 0; x < width; x++) {
               *dest++ = *src++;
               *dest++ = *src++;
               *dest++ = *src++;
               *dest++ = *src++;
            }
         }
         break;

      // Oops!
      default:
         picel_destroy_bitmap(map);
         png_destroy_read_struct(&png_ptr, NULL, NULL);
         picel_get_close_func()(stream);
         return NULL;
   }

   // Success!
   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   picel_get_close_func()(stream);
   return map;
}

//***************************************************************************
// func_read [internal]
// Reading function for libpng
//---------------------------------------------------------------------------
// param png_ptr: pointer to PNG data
// param buffer: where to store data
// param length: amount to read in bytes
//***************************************************************************

static void func_read(png_structp png_ptr, png_bytep buffer, png_size_t
length) {
   // Attempt to read data
   if (picel_get_read_func()(png_get_io_ptr(png_ptr), buffer, length))
      png_error(png_ptr, "");
}
