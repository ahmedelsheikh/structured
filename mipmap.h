/* * structured - Tools for the Generation and Visualization of Large-scale
 * Three-dimensional Reconstructions from Image Data. This software includes
 * source code from other projects, which is subject to different licensing,
 * see COPYING for details. If this project is used for research see COPYING
 * for making the appropriate citations.
 * Copyright (C) 2013 Matthew Johnson-Roberson <mattkjr@gmail.com>
 *
 * This file is part of structured.
 *
 * structured is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * structured is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with structured.  If not, see <http://www.gnu.org/licenses/>.
/*
	DDS GIMP plugin

	Copyright (C) 2004-2012 Shawn Kirst <skirst@gmail.com>,
   with parts (C) 2003 Arne Reuter <homepage@arnereuter.de> where specified.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; see the file COPYING.  If not, write to
	the Free Software Foundation, 51 Franklin Street, Fifth Floor
	Boston, MA 02110-1301, USA.
*/

#ifndef MIPMAP_H
#define MIPMAP_H

int get_num_mipmaps(int width, int height);
unsigned int get_mipmapped_size(int width, int height, int bpp,
                                int level, int num, int format);
unsigned int get_volume_mipmapped_size(int width, int height,
                                       int depth, int bpp, int level,
                                       int num, int format);
int get_next_mipmap_dimensions(int *next_w, int *next_h,
                               int  curr_w, int  curr_h);

float cubic_interpolate(float a, float b, float c, float d, float x);
int generate_mipmaps(unsigned char *dst, unsigned char *src,
                     unsigned int width, unsigned int height, int bpp,
                       int dw, int dh, int filter, int wrap,
                     int gamma_correct, float gamma);
int generate_volume_mipmaps(unsigned char *dst, unsigned char *src,
                            unsigned int width, unsigned int height,
                            unsigned int depth, int bpp, int indexed,
                            int mipmaps, int filter, int wrap,
                            int gamma_correct, float gamma);
void scale_image_boxmax(unsigned char *dst, int dw, int dh,
                                unsigned char *src, int sw, int sh);
typedef enum
{
   DDS_MIPMAP_FILTER_DEFAULT = 0,
   DDS_MIPMAP_FILTER_NEAREST,
   DDS_MIPMAP_FILTER_BOX,
   DDS_MIPMAP_FILTER_TRIANGLE,
   DDS_MIPMAP_FILTER_QUADRATIC,
   DDS_MIPMAP_FILTER_BSPLINE,
   DDS_MIPMAP_FILTER_MITCHELL,
   DDS_MIPMAP_FILTER_LANCZOS,
   DDS_MIPMAP_FILTER_KAISER,
   DDS_MIPMAP_FILTER_MAX
} DDS_MIPMAP_FILTER;

typedef enum
{
   DDS_MIPMAP_WRAP_DEFAULT = 0,
   DDS_MIPMAP_WRAP_MIRROR,
   DDS_MIPMAP_WRAP_REPEAT,
   DDS_MIPMAP_WRAP_CLAMP,
   DDS_MIPMAP_WRAP_MAX
} DDS_MIPMAP_WRAP;
#endif
