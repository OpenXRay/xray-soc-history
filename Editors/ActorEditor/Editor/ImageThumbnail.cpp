#include "stdafx.h"
#pragma hdrstop

#include "ImageThumbnail.h"
#include "ImageManager.h"
#include "xrImage_Resampler.h"

//----------------------------------------------------
bool DrawThumbnail(HDC hdc, U32Vec& data, int offs_x, int offs_y, int dest_w, int dest_h, int src_w, int src_h)
{
    BITMAPINFO bi;
    ZeroMemory					(&bi, sizeof(bi));
    bi.bmiHeader.biSize 		= sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth 		= src_w;
    bi.bmiHeader.biHeight 		= src_h;
    bi.bmiHeader.biPlanes 		= 1;
    bi.bmiHeader.biBitCount 	= 32;
    bi.bmiHeader.biCompression	= BI_RGB;
    bi.bmiHeader.biSizeImage 	= src_w*src_h*4;

    SetStretchBltMode			(hdc, STRETCH_HALFTONE);
    int ln = StretchDIBits		(hdc, offs_x,offs_y, dest_w,dest_h, 0,0, src_w,src_h, (u8*)data.begin(), &bi, DIB_RGB_COLORS, SRCCOPY);
	if (ln==GDI_ERROR){
    	ELog.Msg(mtError,"%s",Engine.LastWindowsError());
    	return false;
    }
	return true;
}
//----------------------------------------------------

LPCSTR EImageThumbnail::FormatString()
{
	LPCSTR c_fmt = 0;
    switch (m_Type){
    case EITObject:
    break;
    case EITTexture:
		for(int i=0; tfmt_token[i].name; i++)
        	if (tfmt_token[i].id==m_TexParams.fmt){
            	c_fmt=tfmt_token[i].name;
                break;
            }
    break;
    }
    return c_fmt;
}
//----------------------------------------------------

int EImageThumbnail::MemoryUsage()
{
	int mem_usage = 0;
    switch (m_Type){
    case EITObject:
    break;
    case EITTexture:{
    	mem_usage = _Width()*_Height()*4;
        switch (m_TexParams.fmt){
        case STextureParams::tfDXT1:
        case STextureParams::tfADXT1: 	mem_usage/=6; break;
        case STextureParams::tfDXT3:
        case STextureParams::tfDXT5: 	mem_usage/=4; break;
        case STextureParams::tf4444:
        case STextureParams::tf1555:
        case STextureParams::tf565: 	mem_usage/=2; break;
        case STextureParams::tfRGBA:	break;
        }
		AnsiString fn 	= ChangeFileExt(m_Name,".seq");
        FS.update_path("$game_textures$",fn);
        if (FS.exist(fn.c_str())){
        	string128		buffer;
			IReader* F		= FS.r_open(0,fn.c_str());
            F->r_string		(buffer);
			int cnt = 0;
            while (!F->eof()){
                F->r_string(buffer);
                cnt++;
            }
            FS.r_close		(F);
	        mem_usage *= cnt?cnt:1;
        }
    }
    break;
    }
    return mem_usage;
}
//----------------------------------------------------

void EImageThumbnail::VFlip()
{
//	return;
	R_ASSERT(!m_Pixels.empty());
	u32 line[THUMB_WIDTH];
    u32 sz_ln=sizeof(u32)*THUMB_WIDTH;
    u32 y2 = THUMB_WIDTH-1;
    for (int y=0; y<THUMB_HEIGHT/2; y++,y2--){
    	CopyMemory(line,m_Pixels.begin()+y2*THUMB_WIDTH,sz_ln);
    	CopyMemory(m_Pixels.begin()+y2*THUMB_WIDTH,m_Pixels.begin()+y*THUMB_WIDTH,sz_ln);
    	CopyMemory(m_Pixels.begin()+y*THUMB_WIDTH,line,sz_ln);
    }
}
//----------------------------------------------------

EImageThumbnail::EImageThumbnail(LPCSTR src_name, THMType type, bool bLoad)
{
	m_Type	= type;
	m_Name 	= ChangeFileExt(src_name,".thm");
    m_Age	= 0;
    if (bLoad) 	if (!Load()&&IsTexture()) ImageManager.CreateTextureThumbnail(this,src_name);
}

EImageThumbnail::~EImageThumbnail()
{
	m_Pixels.clear();
}

void EImageThumbnail::CreateFromData(u32* p, u32 w, u32 h)
{
	R_ASSERT(IsTexture());
	R_ASSERT(p&&(w>0)&&(h>0));
//	imf_filter	imf_box  imf_triangle  imf_bell  imf_b_spline  imf_lanczos3  imf_mitchell
	m_Pixels.resize(THUMB_SIZE);
	imf_Process(m_Pixels.begin(),THUMB_WIDTH,THUMB_HEIGHT,p,w,h,imf_box);
    m_TexParams.width = w;
    m_TexParams.height= h;
    m_TexParams.flags.set(STextureParams::flHasAlpha,FALSE);
}

void EImageThumbnail::CreateFromData(u32* p, u32 w, u32 h, int fc, int vc)
{
	R_ASSERT(p&&(w>0)&&(h>0));
	m_Pixels.resize(THUMB_SIZE);
	imf_Process(m_Pixels.begin(),THUMB_WIDTH,THUMB_HEIGHT,p,w,h,imf_box);
    m_TexParams.vertex_count = vc;
    m_TexParams.face_count	 = fc;
}

bool EImageThumbnail::Load(LPCSTR src_name, LPCSTR path)
{
	AnsiString fn = ChangeFileExt(src_name?AnsiString(src_name):m_Name,".thm");
    if (path) FS.update_path(path,fn);
    else{
	    switch (m_Type){
    	case EITObject: FS.update_path("$objects$",fn); 	break;
	    case EITTexture:FS.update_path("$textures$",fn); 	break;
    	}
    }
    if (!FS.exist(fn.c_str())) return false;
    
//    IReader* F 	= FS.r_open(fn.c_str());
	CCompressedReader* F =xr_new<CCompressedReader>(fn.c_str(),THM_SIGN);
    
    u32 version = 0;

    R_ASSERT(F->r_chunk(THM_CHUNK_VERSION,&version));
    if( version!=THM_CURRENT_VERSION ){
        ELog.Msg( mtError, "Thumbnail: Unsupported version.");
        return false;
    }

    R_ASSERT(F->find_chunk(THM_CHUNK_DATA));
    m_Pixels.resize(THUMB_SIZE);
    F->r(m_Pixels.begin(),THUMB_SIZE*sizeof(u32));

    R_ASSERT(F->find_chunk(THM_CHUNK_TYPE));
    m_Type	= THMType(F->r_u32());

    if (IsTexture()){
        R_ASSERT(F->find_chunk(THM_CHUNK_TEXTUREPARAM));
        F->r					(&m_TexParams.fmt,sizeof(STextureParams::ETFormat));
        m_TexParams.flags.set	(F->r_u32());
        m_TexParams.border_color= F->r_u32();
        m_TexParams.fade_color	= F->r_u32();
        m_TexParams.fade_amount	= F->r_u32();
        m_TexParams.mip_filter	= F->r_u32();
        m_TexParams.width		= F->r_u32();
        m_TexParams.height		= F->r_u32();

        if (F->find_chunk(THM_CHUNK_TEXTURE_TYPE)){
            m_TexParams.type	= (STextureParams::ETType)F->r_u32();
        }

        if (F->find_chunk(THM_CHUNK_DETAIL_EXT)){
            F->r_stringZ			(m_TexParams.detail_name);
            m_TexParams.detail_scale = F->r_float();
        }
    }else{
        if (F->find_chunk(THM_CHUNK_OBJECTPARAM)){
            m_TexParams.face_count 		= F->r_u32();
            m_TexParams.vertex_count 	= F->r_u32();
        }
    }

    m_Age = FS.get_file_age(fn.c_str());

    return true;
}

void EImageThumbnail::Save(int age, LPCSTR path){
	THROW;
	if (!Valid()) return;

    CMemoryWriter F;
	F.open_chunk	(THM_CHUNK_VERSION);
	F.w_u16			(THM_CURRENT_VERSION);
	F.close_chunk	();

	F.w_chunk		(THM_CHUNK_DATA | CFS_CompressMark,m_Pixels.begin(),m_Pixels.size()*sizeof(u32));

    F.open_chunk	(THM_CHUNK_TYPE);
    F.w_u32			(m_Type);
	F.close_chunk	();

	if (IsTexture()){
        F.open_chunk(THM_CHUNK_TEXTUREPARAM);
        F.w			(&m_TexParams.fmt,sizeof(STextureParams::ETFormat));
        F.w_u32		(m_TexParams.flags.get());
        F.w_u32		(m_TexParams.border_color);
        F.w_u32		(m_TexParams.fade_color);
        F.w_u32		(m_TexParams.fade_amount);
        F.w_u32		(m_TexParams.mip_filter);
        F.w_u32		(m_TexParams.width);
        F.w_u32		(m_TexParams.height);
        F.close_chunk	();

        F.open_chunk	(THM_CHUNK_TEXTURE_TYPE);
        F.w_u32		(m_TexParams.type);
        F.close_chunk	();


        F.open_chunk	(THM_CHUNK_DETAIL_EXT);
        F.w_stringZ		(m_TexParams.detail_name);
        F.w_float		(m_TexParams.detail_scale);
        F.close_chunk	();
    }else{
        F.open_chunk	(THM_CHUNK_OBJECTPARAM);
        F.w_u32		(m_TexParams.face_count);
        F.w_u32		(m_TexParams.vertex_count);
        F.close_chunk	();
    }

	AnsiString fn 	= m_Name;
    if (path) FS.update_path(path,fn);
    else{
        switch (m_Type){
        case EITObject: FS.update_path("$objects$",fn); 	break;
        case EITTexture:FS.update_path("$textures$",fn); 	break;
        }
    }
    F.save_to		(fn.c_str());

    FS.set_file_age	(fn.c_str(),age?age:m_Age);
}

void EImageThumbnail::FillProp(PropItemVec& items)
{
	STextureParams& F	= m_TexParams;
	if (IsTexture()){
        PHelper.CreateToken		(items, "Format",					(u32*)&F.fmt, 			tfmt_token,4);
        PHelper.CreateToken		(items, "Type",						(u32*)&F.type,			ttype_token,4);

        PHelper.CreateFlag32	(items, "MipMaps\\Enabled",			&F.flags,				STextureParams::flGenerateMipMaps);
        PHelper.CreateToken		(items, "MipMaps\\Filter",			&F.mip_filter,			tparam_token,4);

        PHelper.CreateFlag32	(items, "Details\\Enabled",			&F.flags,				STextureParams::flHasDetailTexture);
        PHelper.CreateTexture	(items, "Details\\Texture",			F.detail_name,			sizeof(F.detail_name));
        PHelper.CreateFloat		(items, "Details\\Scale",			&F.detail_scale,		0.1f,10000.f,0.1f,2);

        PHelper.CreateFlag32	(items, "Flags\\Grayscale",			&F.flags,				STextureParams::flGreyScale);
        PHelper.CreateFlag32	(items, "Flags\\Binary Alpha",		&F.flags,				STextureParams::flBinaryAlpha);
        PHelper.CreateFlag32	(items, "Flags\\Dither",			&F.flags,				STextureParams::flDitherColor);
        PHelper.CreateFlag32	(items, "Flags\\Dither Each MIP",	&F.flags,				STextureParams::flDitherEachMIPLevel);
        PHelper.CreateFlag32	(items, "Flags\\Implicit Lighted",	&F.flags,				STextureParams::flImplicitLighted);

        PHelper.CreateFlag32	(items, "Fade\\Enabled Color",		&F.flags,				STextureParams::flFadeToColor);
        PHelper.CreateFlag32	(items, "Fade\\Enabled Alpha",		&F.flags,				STextureParams::flFadeToAlpha);
        PHelper.CreateU32		(items, "Fade\\Amount",				&F.fade_amount,			0,1000,0);
        PHelper.CreateColor		(items, "Fade\\Color",				&F.fade_color			);

        PHelper.CreateFlag32	(items, "Border\\Enabled Color",	&F.flags,				STextureParams::flColorBorder);
        PHelper.CreateFlag32	(items, "Border\\Enabled Alpha",	&F.flags,				STextureParams::flAlphaBorder);
        PHelper.CreateColor		(items, "Border\\Color",			&F.border_color			);
    }else{
        PHelper.CreateCaption	(items, "Face Count",				AnsiString(F.face_count).c_str());
        PHelper.CreateCaption	(items, "Vertex Count",				AnsiString(F.vertex_count).c_str());
    }
}

void EImageThumbnail::Draw(TCanvas* pCanvas, const TRect& R, bool bStretch)
{
	if (IsTexture()){
        TRect r = R; r.left += 1; r.top += 1;
        float w, h;
        w = _Width();
        h = _Height();
        if (w!=h)	pCanvas->FillRect(R);
        if (w>h){   r.right = R.left + R.Width()-1; 	r.bottom = R.top + h/w*R.Height()-1;
        }else{      r.right = R.left + w/h*R.Width()-1; r.bottom = R.top + R.Height()-1;}
        DrawThumbnail(pCanvas->Handle,m_Pixels,r.left,r.top,r.Width(),r.Height(),THUMB_WIDTH,THUMB_HEIGHT);
    }else{
        TRect r = R; r.left += 1; r.top += 1;
        r.right -= 1; r.bottom -= 1;
        DrawThumbnail(pCanvas->Handle,m_Pixels,r.left,r.top,r.Width(),r.Height(),THUMB_WIDTH,THUMB_HEIGHT);
    }
}

void EImageThumbnail::Draw(TPanel* panel, TPaintBox* pbox, bool bStretch)
{
	if (IsTexture()){
        RECT r;
        r.left = 2; r.top = 2;
        float w, h;
        w = _Width();
        h = _Height();
        if (w!=h)	pbox->Canvas->FillRect(pbox->BoundsRect);
        if (w>h){   r.right = pbox->Width-1; r.bottom = h/w*pbox->Height-1;
        }else{      r.right = w/h*pbox->Width-1; r.bottom = pbox->Height-1;}
        HDC hdc 	= GetDC	(panel->Handle);
        DrawThumbnail(hdc,m_Pixels,r.left,r.top,r.right-r.left,r.bottom-r.top,THUMB_WIDTH,THUMB_HEIGHT);
        ReleaseDC	(panel->Handle,hdc);
    }else{
        RECT r;		r.left = 2; r.top = 2;
        r.right 	= pbox->Width-1; r.bottom = pbox->Height-1;
        HDC hdc 	= GetDC	(panel->Handle);
        DrawThumbnail(hdc,m_Pixels,r.left,r.top,r.right-r.left,r.bottom-r.top,THUMB_WIDTH,THUMB_HEIGHT);
        ReleaseDC	(panel->Handle,hdc);
    }
}


