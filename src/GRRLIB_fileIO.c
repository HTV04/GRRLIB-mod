/*------------------------------------------------------------------------------
Copyright (c) 2009-2022 The GRRLIB Team and HTV04

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
------------------------------------------------------------------------------*/
#include <grrlib-mod.h>
#include <stdio.h>
#include <pngu-mod.h>

/**
 * Load a file to memory.
 * @param filename Name of the file to be loaded.
 * @param data Pointer-to-your-pointer.
 * Ie. { u8 *data; GRRLIB_LoadFile("file", &data); }.
 * It is your responsibility to free the memory allocated by this function.
 * @return A integer representing a code:
 *         -     0 : EmptyFile.
 *         -    -1 : FileNotFound.
 *         -    -2 : OutOfMemory.
 *         -    -3 : FileReadError.
 *         -    >0 : FileLength.
 */
int  GRRLIB_LoadFile(const char* filename, u8* *data) {
	int   len;
	FILE  *fd;

	// Open the file
	if ( !(fd = fopen(filename, "rb")) ) {
		return -1;
	}

	// Get file length
	fseek(fd, 0, SEEK_END);
	if ( !(len = ftell(fd)) ) {
		fclose(fd);
		*data = NULL;
		return 0;
	}
	fseek(fd, 0, SEEK_SET);

	// Grab some memory in which to store the file
	if ( (*data = malloc(len)) == NULL ) {
		fclose(fd);
		return -2;
	}

	if ( fread(*data, 1, len, fd) != len) {
		fclose(fd);
		free(*data);
		*data = NULL;
		return -3;
	}

	fclose(fd);
	return len;
}

/**
 * Load a texture from a file.
 * @param filename The JPEG, PNG or Bitmap filename to load.
 * @return A GRRLIB_texImg structure filled with image information.
 *         If an error occurs NULL will be returned.
 */
GRRLIB_texture*  GRRLIB_LoadTextureFromFile(const char *filename) {
	GRRLIB_texture  *tex;
	u8              *data;
	int              len;

	// Return NULL if load fails
	len = GRRLIB_LoadFile(filename, &data);
	if (len <= 0) {
		return NULL;
	}

	// Convert to texture
	tex = GRRLIB_LoadTextureEx(data, len);
	if (tex == NULL) {
		tex = GRRLIB_LoadTexture(data);
	}

	// Free up the buffer
	free(data);

	return tex;
}

/**
 * Load a TTF from a file.
 * @param filename The TTF filename to load.
 * @return A GRRLIB_ttfFont structure filled with font information.
 *         If an error occurs NULL will be returned.
 */
GRRLIB_ttfFont*  GRRLIB_LoadTTFFromFile(const char *filename) {
	GRRLIB_ttfFont  *ttf;
	u8              *data;

	s32 size = GRRLIB_LoadFile(filename, &data);

	// Return NULL if load fails
	if (size <= 0) {
		return NULL;
	}

	// Convert to TTF
	ttf = GRRLIB_LoadTTF(data, size);

	// Free up the buffer
	free(data);

	return ttf;
}

/**
 * Make a PNG screenshot.
 * It should be called after drawing stuff on the screen, but before GRRLIB_Render().
 * @param filename Name of the file to write.
 * @return Returns @c true if everything worked, @c false if problems occurred.
 */
bool  GRRLIB_ScrShot(const char* filename) {
	int     ret = -1;
	IMGCTX  pngContext = PNGU_SelectImageFromDevice(filename);

	if ( pngContext != NULL ) {
		ret = PNGU_EncodeFromEFB( pngContext,
		                          GRRLIB_VideoMode->fbWidth, GRRLIB_VideoMode->efbHeight,
		                          0 );
		PNGU_ReleaseImageContext(pngContext);
	}
	return !ret;
}
