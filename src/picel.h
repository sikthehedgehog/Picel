//***************************************************************************
// "picel.h"
// Main header for the Picel library, defines all prototypes and such
// Any program that wants to make use of Picel should include this header
//***************************************************************************

#ifndef PICEL_H
#define PICEL_H

// Required headers
#include <stddef.h>
#include <stdint.h>

// C++ shenanigans
#ifdef __cplusplus
extern "C" {
#endif

// Object type declarations
typedef struct _PicelBitmap PicelBitmap;

// Possible stream opening modes
typedef enum {
   PICEL_OPEN_READ,
   PICEL_OPEN_WRITE
} PicelOpenMode;

// Possible seeking modes
// Yes, these are equivalent to their standard library counterparts...
// I can't rely on their value being the same across systems though
typedef enum {
   PICEL_SEEK_SET,
   PICEL_SEEK_CUR,
   PICEL_SEEK_END
} PicelSeekMode;

// Prototypes for stream handling functions
typedef void *PicelOpenFunc(const char *, PicelOpenMode);
typedef int PicelReadFunc(void *, void *, size_t);
typedef int PicelSeekFunc(void *, long, PicelSeekMode);
typedef int PicelWriteFunc(void *, const void *, size_t);
typedef void PicelCloseFunc(void *);

// Bitmap creation
PicelBitmap *picel_create_bitmap(int, int, int);
PicelBitmap *picel_load_bitmap(const char *);
PicelBitmap *picel_load_png(const char *);
int picel_save_png(const char *, const PicelBitmap *);
void picel_destroy_bitmap(PicelBitmap *);

// Bitmap editing
uint32_t picel_get_pixel(const PicelBitmap *, int, int);
void picel_set_pixel(PicelBitmap *, int, int, uint32_t);
uint32_t picel_get_raw_pixel(const PicelBitmap *, int, int);
void picel_set_raw_pixel(PicelBitmap *, int, int, uint32_t);
uint32_t picel_get_color(const PicelBitmap *, uint8_t);
void picel_set_color(PicelBitmap *, uint8_t, uint32_t);
uint8_t *picel_get_raw_data_ptr(PicelBitmap *);

// Bitmap properties
int picel_get_width(const PicelBitmap *);
int picel_get_height(const PicelBitmap *);
int picel_is_paletted(const PicelBitmap *);
int picel_convert_bitmap(PicelBitmap *, int, int, int);

// Stream functions
void picel_set_open_func(PicelOpenFunc *);
PicelOpenFunc *picel_get_open_func(void);
void picel_set_read_func(PicelReadFunc *);
PicelReadFunc *picel_get_read_func(void);
void picel_set_seek_func(PicelSeekFunc *);
PicelSeekFunc *picel_get_seek_func(void);
void picel_set_write_func(PicelWriteFunc *);
PicelWriteFunc *picel_get_write_func(void);
void picel_set_close_func(PicelCloseFunc *);
PicelCloseFunc *picel_get_close_func(void);

// C++ shenanigans
#ifdef __cplusplus
}
#endif
#endif
