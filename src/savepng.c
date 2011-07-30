//***************************************************************************
// "savepng.c"
// Code for saving a bitmap into a PNG stream
//***************************************************************************

// Required headers
#include <setjmp.h>
#include <png.h>
#include "picel.h"
#include "bitmap.h"

// Callback prototypes
static void func_write(png_structp, png_bytep, png_size_t);
static void func_flush(png_structp);

//***************************************************************************
// picel_save_png
// Saves a bitmap into a PNG
//---------------------------------------------------------------------------
// param map: pointer to bitmap
// param name: name of stream
// return: zero on success or non-zero on failure
//***************************************************************************

int picel_save_png(const char *name, const PicelBitmap *map) {
   // Try to open stream
   void *stream = picel_get_open_func()(name, 1);
   if (stream == NULL)
      return -1;

   // Create write structure
   png_structp png_ptr = png_create_write_struct
      (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      picel_get_close_func()(stream);
      return -1;
   }

   // Create info structure
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL) {
      png_destroy_write_struct(&png_ptr, NULL);
      picel_get_close_func()(stream);
      return -1;
   }

   // Setjmp shenanigans (ugh)
   if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_write_struct(&png_ptr, &info_ptr);
      picel_get_close_func()(stream);
      return -1;
   }

   // Set up I/O functions
   png_set_write_fn(png_ptr, stream, func_write, func_flush);

   // Set up PNG properties
   png_set_IHDR(png_ptr, info_ptr, map->width, map->height, 8,
      map->paletted ? PNG_COLOR_TYPE_PALETTE : PNG_COLOR_TYPE_RGB_ALPHA,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT);

   // Set up PNG palette if needed
   if (map->paletted) {
      // We'll store the PNG colors here
      png_color palette[0x100];

      // Fill palette table
      int i;
      for (i = 0; i < 0x100; i++) {
         uint32_t color = map->pal[i];

         palette[i].red   = color >> 24;
         palette[i].green = color >> 16;
         palette[i].blue  = color >> 8;
      }

      // Set palette
      png_set_PLTE(png_ptr, info_ptr, palette, 0x100);
   }

   // Tell libpng where to find the bitmap data
   // (for the record, we already have a row table, so use it!)
   png_set_rows(png_ptr, info_ptr, map->rows);

   // Write it!
   png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

   // Success!
   png_destroy_write_struct(&png_ptr, &info_ptr);
   picel_get_close_func()(stream);
   return 0;
}

//***************************************************************************
// func_write [internal]
// Writing function for libpng
//---------------------------------------------------------------------------
// param png_ptr: pointer to PNG data
// param buffer: data to be written
// param length: amount to write in bytes
//***************************************************************************

static void func_write(png_structp png_ptr, png_bytep buffer, png_size_t
length) {
   // Attempt to write data
   if (picel_get_write_func()(png_get_io_ptr(png_ptr), buffer, length))
      png_error(png_ptr, "");
}

//***************************************************************************
// func_flush [internal]
// Flushing function for libpng
//---------------------------------------------------------------------------
// param png_ptr: pointer to PNG data
//***************************************************************************

static void func_flush(png_structp png_ptr) {
}
