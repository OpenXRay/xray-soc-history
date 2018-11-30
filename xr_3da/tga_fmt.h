//-----------------------------------------------------------------------------
// Descr....: Targa File Format Specific Parameters
//-----------------------------------------------------------------------------
        
#ifndef _TGAFMTSPEC_
#define _TGAFMTSPEC_

//-- Targa write types

#define TGA_16_BITS 0			//-- Alpha bit set if alpha > 0
#define TGA_24_BITS 1
#define TGA_32_BITS 2

//-----------------------------------------------------------------------------
//-- TGA data Structure -------------------------------------------------------
//

#define TGAVERSION 103

typedef struct _tgauserdata {
	DWORD	version;			//-- Reserved
	BOOL	saved;				//-- Reserved
	BYTE	writeType;			//-- Write type define above
	BYTE	compressed;			//-- Compression (TRUE or FALSE)
	BOOL	alphaSplit;			//-- Write separate Alpha File (TRUE or FALSE)
	BOOL	preMultAlpha;		//-- Write premultiplied alpha (TRUE or FALSE)
	TCHAR	author[41];			//-- Author name
	TCHAR	jobname[41];		//-- Job Name
	TCHAR	comments1[81];		//-- Comments
	TCHAR	comments2[81];
	TCHAR	comments3[81];
	TCHAR	comments4[81];
} TGAUSERDATA;

#endif

//-- EOF: fmtspec.h -----------------------------------------------------------
