/* config1a.h -- configuration for the LZO1A algorithm

   This file is part of the LZO real-time data compression library.

   Copyright (C) 1996-2002 Markus Franz Xaver Johannes Oberhumer
   All Rights Reserved.

   The LZO library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The LZO library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the LZO library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer
   <markus@oberhumer.com>
 */


/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the library and is subject
   to change.
 */


#ifndef __LZO_CONFIG1A_H
#define __LZO_CONFIG1A_H

#include "lzo1a.h"
#include "lzo_conf.h"

#undef LZO_COLLECT_STATS	/* no support for stats here */


/***********************************************************************
// algorithm configuration
************************************************************************/

/* run bits (4 - 5) - the compressor and the decompressor
 * must use the same value. */
#if !defined(RBITS)
#  define RBITS		5
#endif

/* dictionary depth (0 - 6) - this only affects the compressor.
 * 0 is fastest, 6 is best compression ratio */
#if !defined(DDBITS)
#  define DDBITS	0
#endif

/* compression level (1 - 9) - this only affects the compressor.
 * 1 is fastest, 9 is best compression ratio */
#if !defined(CLEVEL)
#  define CLEVEL	1			/* fastest by default */
#endif


/* check configuration */
#if (RBITS < 4 || RBITS > 5)
#  error "invalid RBITS"
#endif
#if (DDBITS < 0 || DDBITS > 6)
#  error "invalid DDBITS"
#endif
#if (CLEVEL < 1 || CLEVEL > 9)
#  error "invalid CLEVEL"
#endif


/***********************************************************************
// internal configuration
************************************************************************/

/* add a special code so that the decompressor can detect the
 * end of the compressed data block (overhead is 3 bytes per block) */
#undef LZO_EOF_CODE

/* return -1 instead of copying if the data cannot be compressed */
#undef LZO_RETURN_IF_NOT_COMPRESSIBLE


/***********************************************************************
// algorithm internal configuration
************************************************************************/

/* choose the hashing strategy */
#ifndef LZO_HASH
#define LZO_HASH		LZO_HASH_LZO_INCREMENTAL_A
#endif

/* config */
#define R_BITS			RBITS
#define DD_BITS			DDBITS
#ifndef D_BITS
#define D_BITS			16
#endif


/***********************************************************************
// optimization and debugging
************************************************************************/

/* Collect statistics */
#if 0 && !defined(LZO_COLLECT_STATS)
#  define LZO_COLLECT_STATS
#endif


/***********************************************************************
//
************************************************************************/

#define M3O_BITS		M2O_BITS
#define M3L_BITS		CHAR_BIT
#define M3_MAX_LEN		(M3_MIN_LEN + LZO_SIZE(M3L_BITS) - 1)
#define _MAX_OFFSET		_M2_MAX_OFFSET
#define LZO_NO_M3

#include "lzo_util.h"
#include "lzo1b_de.h"
#include "stats1b.h"

#include "lzo1b_cc.h"


/***********************************************************************
// check for total LZO1/LZO1A compatibility
************************************************************************/

#undef M2_MARKER
#define M2_MARKER	(1 << M2O_BITS)

#if (R_BITS != 5)
#  error
#endif
#if (M2O_BITS != 5)
#  error
#endif
#if (M3O_BITS != 5)
#  error
#endif
#if (M2_MIN_LEN != 3)
#  error
#endif
#if (M2_MAX_LEN != 8)
#  error
#endif
#if (M3_MIN_LEN != 9)
#  error
#endif
#if (M3_MAX_LEN != 264)
#  error
#endif
#if (_M2_MAX_OFFSET != (1u << 13))
#  error
#endif
#if (_M2_MAX_OFFSET != _M3_MAX_OFFSET)
#  error
#endif
#if (_M2_MAX_OFFSET != _MAX_OFFSET)
#  error
#endif
#if (R0MIN != 32)
#  error
#endif
#if (R0MAX != 287)
#  error
#endif
#if (R0FAST != 280)
#  error
#endif


#endif /* already included */

/*
vi:ts=4:et
*/

