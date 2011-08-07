//***************************************************************************
// "stream.c"
// Callbacks for accessing data in streams (usually files)
//***************************************************************************

// Required headers
#include <stddef.h>
#include <stdio.h>
#include "picel.h"

// Prototypes for the default functions for handling streams
static void *defopenfunc(const char *, PicelOpenMode);
static int defreadfunc(void *, void *, size_t);
static int defseekfunc(void *, long, PicelSeekMode);
static int defwritefunc(void *, const void *, size_t);
static void defclosefunc(void *);

// Pointers to the functions to be used when handling streams
static PicelOpenFunc *openfunc = defopenfunc;
static PicelReadFunc *readfunc = defreadfunc;
static PicelSeekFunc *seekfunc = defseekfunc;
static PicelWriteFunc *writefunc = defwritefunc;
static PicelCloseFunc *closefunc = defclosefunc;

//***************************************************************************
// picel_set_open_func
// Changes the function used for opening streams
//---------------------------------------------------------------------------
// param func: pointer to function or NULL for default
//***************************************************************************

void picel_set_open_func(PicelOpenFunc *func) {
   // If a function was specified, use it for opening streams, otherwise use
   // the default function instead
   openfunc = func ? func : defopenfunc;
}

//***************************************************************************
// picel_get_open_func
// Retrieves the function used for opening streams
//---------------------------------------------------------------------------
// return: pointer to function
//***************************************************************************

PicelOpenFunc *picel_get_open_func(void) {
   return openfunc;
}

//***************************************************************************
// picel_set_read_func
// Changes the function used for reading streams
//---------------------------------------------------------------------------
// param func: pointer to function or NULL for default
//***************************************************************************

void picel_set_read_func(PicelReadFunc *func) {
   // If a function was specified, use it for reading streams, otherwise use
   // the default function instead
   readfunc = func ? func : defreadfunc;
}

//***************************************************************************
// picel_get_read_func
// Retrieves the function used for reading streams
//---------------------------------------------------------------------------
// return: pointer to function
//***************************************************************************

PicelReadFunc *picel_get_read_func(void) {
   return readfunc;
}

//***************************************************************************
// picel_set_seek_func
// Changes the function used for seeking in streams
//---------------------------------------------------------------------------
// param func: pointer to function or NULL for default
//***************************************************************************

void picel_set_seek_func(PicelSeekFunc *func) {
   // If a function was specified, use it for seeking in streams, otherwise
   // use the default function instead
   seekfunc = func ? func : defseekfunc;
}

//***************************************************************************
// picel_get_seek_func
// Retrieves the function used for seeking in streams
//---------------------------------------------------------------------------
// return: pointer to function
//***************************************************************************

PicelSeekFunc *picel_get_seek_func(void) {
   return seekfunc;
}

//***************************************************************************
// picel_set_write_func
// Changes the function used for writing streams
//---------------------------------------------------------------------------
// param func: pointer to function or NULL for default
//***************************************************************************

void picel_set_write_func(PicelWriteFunc *func) {
   // If a function was specified, use it for writing streams, otherwise use
   // the default function instead
   writefunc = func ? func : defwritefunc;
}

//***************************************************************************
// picel_get_write_func
// Retrieves the function used for writing streams
//---------------------------------------------------------------------------
// return: pointer to function
//***************************************************************************

PicelWriteFunc *picel_get_write_func(void) {
   return writefunc;
}

//***************************************************************************
// picel_set_close_func
// Changes the function used for closing streams
//---------------------------------------------------------------------------
// param func: pointer to function or NULL for default
//***************************************************************************

void picel_set_close_func(PicelCloseFunc *func) {
   // If a function was specified, use it for closing streams, otherwise use
   // the default function instead
   closefunc = func ? func : defclosefunc;
}

//***************************************************************************
// picel_get_close_func
// Retrieves the function used for closing streams
//---------------------------------------------------------------------------
// return: pointer to function
//***************************************************************************

PicelCloseFunc *picel_get_close_func(void) {
   return closefunc;
}

//***************************************************************************
// defopenfunc [internal]
// Default function for opening a stream
//---------------------------------------------------------------------------
// param name: name of stream
// param mode: zero for read, non-zero for write
// return: stream handle or NULL on failure
//***************************************************************************

static void *defopenfunc(const char *name, PicelOpenMode mode) {
   // Determine mode to use
   const char *fmode;
   switch (mode) {
      case PICEL_OPEN_READ: fmode = "rb"; break;
      case PICEL_OPEN_WRITE: fmode = "wb"; break;
      default: return NULL;
   }

   // Use stdio to open a file
   return fopen(name, fmode);
}

//***************************************************************************
// defreadfunc [internal]
// Default function for reading from a stream
//---------------------------------------------------------------------------
// param stream: stream handle
// param buffer: where to store data
// param size: amount of bytes to read
// return: zero on success, non-zero on failure
//***************************************************************************

static int defreadfunc(void *stream, void *buffer, size_t size) {
   // Read from the file into the buffer
   return fread(buffer, 1, size, (FILE *) stream) != size;
}

//***************************************************************************
// defseekfunc [internal]
// Default function for seeking in a stream
//---------------------------------------------------------------------------
// param stream: stream handle
// param offset: target position
// param mode: from where to seek
// return: zero on success, non-zero on failure
//***************************************************************************

static int defseekfunc(void *stream, long offset, PicelSeekMode mode) {
   // Determine which seeking mode to use in the standard library
   int stdmode;
   switch (mode) {
      case PICEL_SEEK_SET: stdmode = SEEK_SET;
      case PICEL_SEEK_CUR: stdmode = SEEK_CUR;
      case PICEL_SEEK_END: stdmode = SEEK_END;
      default: return -1;
   }

   // Seek into the specified position
   return fseek((FILE *) stream, offset, mode);
}

//***************************************************************************
// defwritefunc [internal]
// Default function for writing into a stream
//---------------------------------------------------------------------------
// param stream: stream handle
// param buffer: data to be written
// param size: amount of bytes to write
// return: zero on success, non-zero on failure
//***************************************************************************

static int defwritefunc(void *stream, const void *buffer, size_t size) {
   // Write into the file from the buffer
   return fwrite(buffer, 1, size, (FILE *) stream) != size;
}

//***************************************************************************
// defclosefunc [internal]
// Default function for closing a stream
//---------------------------------------------------------------------------
// param stream: stream handle
//***************************************************************************

static void defclosefunc(void *stream) {
   // Close file
   fclose((FILE *) stream);
}
