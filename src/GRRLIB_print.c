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

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>

#include <grrlib-mod.h>

/**
 * Print formatted output.
 * @param xpos Specifies the x-coordinate of the upper-left corner of the text.
 * @param ypos Specifies the y-coordinate of the upper-left corner of the text.
 * @param tex The texture containing the character set.
 * @param zoom This is a factor by which the text size will be increase or decrease.
 * @param text Text to draw.
 * @param ... Optional arguments.
 */
void  GRRLIB_Printf (const f32 xpos, const f32 ypos,
                     const GRRLIB_texImg *tex, const f32 zoom,
                     const char *text, ...) {
	if (tex == NULL || tex->data == NULL) {
		return;
	}

	int i, size;
	char tmp[1024];
	f32 offset = tex->tilew * zoom;

	va_list argp;
	va_start(argp, text);
	size = vsnprintf(tmp, sizeof(tmp), text, argp);
	va_end(argp);

	for (i = 0; i < size; i++) {
		GRRLIB_DrawTile(xpos+i*offset, ypos, tex, 0, zoom, zoom, 0, 0,
			tmp[i] - tex->tilestart);
	}
}

/**
 * Print formatted output with a ByteMap font.
 * This function could be slow, it should be used with GRRLIB_CompoStart and GRRLIB_CompoEnd.
 * @param xpos Specifies the x-coordinate of the upper-left corner of the text.
 * @param ypos Specifies the y-coordinate of the upper-left corner of the text.
 * @param bmf The ByteMap font to use.
 * @param text Text to draw.
 * @param ... Optional arguments.
 */
void  GRRLIB_PrintBMF (const f32 xpos, const f32 ypos,
                       const GRRLIB_bytemapFont *bmf,
                       const char *text, ...) {
	u32   i, size;
	u8    x, y;
	char  tmp[1024];
	f32   xoff = xpos;
	const GRRLIB_bytemapChar *pchar;

	va_list argp;
	va_start(argp, text);
	size = vsnprintf(tmp, sizeof(tmp), text, argp);
	va_end(argp);

	for (i=0; i<size; i++) {
		pchar = &bmf->charDef[(u8)tmp[i]];
		u8 *pdata = pchar->data;
		for (y=0; y<pchar->height; y++) {
			for (x=0; x<pchar->width; x++) {
				if (*pdata) {
					GX_Begin(GX_POINTS, GX_VTXFMT0, 1);
						GX_Position3f32(xoff + x + pchar->relx, ypos + y + pchar->rely, 0.0f);
						GX_Color1u32(bmf->palette[*pdata]);
					GX_End();
				}
				pdata++;
			}
		}
		xoff += pchar->kerning + bmf->tracking;
	}
}
