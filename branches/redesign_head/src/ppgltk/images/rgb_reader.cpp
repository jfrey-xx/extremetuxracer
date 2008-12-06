/* 
 * PPRacer 
 * Copyright (C) 2004-2005 Volker Stroebel <volker@planetpenguin.de>
 *
 * Copyright (C) 1999-2001 Jasmin F. Patry
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/* This code taken from Mesa 3Dfx demos by David Bucciarelli (tech.hmw@plus.it)
 */
 
#include "rgb_reader.h"

 
#define IMAGIC      0x01da
#define IMAGIC_SWAP 0xda01

#define SWAP_SHORT_BYTES(x) ((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))
#define SWAP_LONG_BYTES(x) (((((x) & 0xff) << 24) | (((x) & 0xff00) << 8)) | \
((((x) & 0xff0000) >> 8) | (((x) & 0xff000000) >> 24)))

namespace pp{ 
 
ReaderRGB::ReaderRGB(const char* fileName)
{
	ImageInfo *image;
	//IMAGEInfo *final;
	int sx;

	image = ImageOpen(fileName);

	if ( image == NULL ) {
		return;
	}

	width=image->sizeX; 
	height= image->sizeY;
	depth= image->sizeZ;

  /* 
   * Round up so rows are long-word aligned 
   */
  sx = ( (image->sizeX) * (image->sizeZ) + 3) >> 2;

  data 
    = (unsigned char *)malloc( sx * image->sizeY * sizeof(unsigned int));

  if (data == NULL) 
    {
      fprintf(stderr, "Out of memory!\n");
      //winsys_exit(-1);
	  return;
    }

  ImageGetRawData(image, data);
  ImageClose(image);
}

ReaderRGB::ImageInfo*
ReaderRGB::ImageOpen(const char *fileName)
{
  ImageInfo *image;
  unsigned int *rowStart, *rowSize, ulTmp;
  int x, i;

  image = (ImageInfo *)malloc(sizeof(ImageInfo));
  if (image == NULL) 
    {
      fprintf(stderr, "Out of memory!\n");
      //winsys_exit(-1);
    }
  if ((image->file = fopen(fileName, "rb")) == NULL) 
    {
      free( image );
      return NULL;
    }
  /*
   *	Read the image header
   */
  fread(image, 1, 12, image->file);
  /*
   *	Check byte order
   */
  if (image->imagic == IMAGIC_SWAP) 
    {
      image->type = SWAP_SHORT_BYTES(image->type);
      image->dim = SWAP_SHORT_BYTES(image->dim);
      image->sizeX = SWAP_SHORT_BYTES(image->sizeX);
      image->sizeY = SWAP_SHORT_BYTES(image->sizeY);
      image->sizeZ = SWAP_SHORT_BYTES(image->sizeZ);
    }

  for ( i = 0 ; i <= image->sizeZ ; i++ )
    {
      image->tmp[i] = (unsigned char *)malloc(image->sizeX*256);
      if (image->tmp[i] == NULL ) 
	{
	  fprintf(stderr, "Out of memory!\n");
	  //winsys_exit(-1);
	}
    }

  if ((image->type & 0xFF00) == 0x0100) /* RLE image */
    {
      x = image->sizeY * image->sizeZ * sizeof(int);
      image->rowStart = (unsigned int *)malloc(x);
      image->rowSize = (unsigned int *)malloc(x);
      if (image->rowStart == NULL || image->rowSize == NULL) 
	{
	  fprintf(stderr, "Out of memory!\n");
	  //winsys_exit(-1);
	}
      image->rleEnd = 512 + (2 * x);
      fseek(image->file, 512, SEEK_SET);
      fread(image->rowStart, 1, x, image->file);
      fread(image->rowSize, 1, x, image->file);
      if (image->imagic == IMAGIC_SWAP) 
	{
	  x /= sizeof(int);
	  rowStart = image->rowStart;
	  rowSize = image->rowSize;
	  while (x--) 
	    {
	      ulTmp = *rowStart;
	      *rowStart++ = SWAP_LONG_BYTES(ulTmp);
	      ulTmp = *rowSize;
	      *rowSize++ = SWAP_LONG_BYTES(ulTmp);
	    }
	}
    }
  return image;
}

void
ReaderRGB::ImageGetRawData( ImageInfo *image, unsigned char *data)
{
  int i, j, k;
  int remain = 0;

  switch ( image->sizeZ )
    {
    case 1:
      remain = image->sizeX % 4;
      break;
    case 2:
      remain = image->sizeX % 2;
      break;
    case 3:
      remain = (image->sizeX * 3) & 0x3;
      if (remain)
	remain = 4 - remain;
      break;
    case 4:
      remain = 0;
      break;
    }

  for (i = 0; i < image->sizeY; i++) 
    {
      for ( k = 0; k < image->sizeZ ; k++ )
	ImageGetRow(image, image->tmp[k+1], i, k);
      for (j = 0; j < image->sizeX; j++) 
	for ( k = 1; k <= image->sizeZ ; k++ )
	  *data++ = *(image->tmp[k] + j);
      data += remain;
    }
}

void
ReaderRGB::ImageGetRow( ImageInfo *image, unsigned char *buf, int y, int z)
{
  unsigned char *iPtr, *oPtr, pixel;
  int count;

  if ((image->type & 0xFF00) == 0x0100)  /* RLE image */
    {
      fseek(image->file, image->rowStart[y+z*image->sizeY], SEEK_SET);
      fread(image->tmp[0], 1, (unsigned int)image->rowSize[y+z*image->sizeY],
	    image->file);

      iPtr = image->tmp[0];
      oPtr = buf;
      while (1) 
	{
	  pixel = *iPtr++;
	  count = (int)(pixel & 0x7F);
	  if (!count)
	    return;
	  if (pixel & 0x80) 
	    {
	      while (count--) 
		{
		  *oPtr++ = *iPtr++;
		}
	    } 
	  else 
	    {
	      pixel = *iPtr++;
	      while (count--) 
		{
		  *oPtr++ = pixel;
		}
	    }
	}
    }
  else /* verbatim image */
    {
      fseek(image->file, 512+(y*image->sizeX)+(z*image->sizeX*image->sizeY),
	    SEEK_SET);
      fread(buf, 1, image->sizeX, image->file);
    }
}

void
ReaderRGB::ImageClose( ImageInfo *image)
{
  int i;

  fclose(image->file);
  for ( i = 0 ; i <= image->sizeZ ; i++ )
    free(image->tmp[i]);
  free(image);
}
 
} //namespace pp
