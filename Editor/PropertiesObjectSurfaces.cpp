#include "stdafx.h"
#pragma hdrstop

#include "PropertiesObject.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "Texture.h"
#include "xrShader.h"
#include "Shader.h"
#include "ui_main.h"
#include "scene.h"
#include "XRShaderDef.h"
#include "main.h"
#include "ChoseForm.h"
#include "xr_trims.h"
//---------------------------------------------------------------------------
// surfaces
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsSurfacesShow(TObject *Sender)
{
	if (!m_CurrentObject) return;
// Set up surfaces&textures
    bool bEq;
    AnsiString name;
    tvSurfaces->Items->Clear();

    // create root tree node (object name)
    name.sprintf("%s (%s)",m_CurrentObject->GetName(),m_CurrentObject->GetName());
    TElTreeItem* root = tvSurfaces->Items->AddObject(0,name,(TObject*)m_CurrentObject);
    root->ParentStyle = false;
    root->Bold = true;
    root->Underlined = true;

    for (SurfaceIt s_it=m_EditObject->m_Surfaces.begin(); s_it!=m_EditObject->m_Surfaces.end(); s_it++){
        TElTreeItem* pNode = tvSurfaces->Items->AddChildObject(root,(*s_it)->name,(TObject*)(*s_it));
        pNode->ParentStyle = false;
        pNode->Bold = true;
        for (AStringIt n_it=(*s_it)->textures.begin(); n_it!=(*s_it)->textures.end(); n_it++)
            tvSurfaces->Items->AddChild(pNode,*n_it);
    }
    tvSurfaces->FullExpand();

    tvSurfaces->Sort(true);
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebSelectShaderClick(TObject *Sender)
{
	if (!surf_selected) return;
	LPCSTR S = TfrmChoseItem::SelectShader(true,0,surf_selected->shader?surf_selected->shader->shader->cName:0);
    if (S){
        lbShader->Caption = S;
    	if (surf_selected->shader){
        	if(strcmp(surf_selected->shader->shader->cName,S)!=0){
		    	Device.Shader.Delete(surf_selected->shader);
    		    surf_selected->shader = Device.Shader.Create(S,surf_selected->textures);
        		OnModified(Sender);
            }
        }else{
            surf_selected->shader = Device.Shader.Create(S,surf_selected->textures);
            OnModified(Sender);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebSelectTextureClick(TObject *Sender)
{
	if (!surf_selected) return;
    AnsiString temp;
    temp = ListToSequence(surf_selected->textures);
	LPCSTR S = TfrmChoseItem::SelectTexture(true,temp.c_str());
    if (S){
        lbTexture->Caption = S;
        SequenceToList(surf_selected->textures,S);
    	if (surf_selected->shader){
			Device.Shader.Delete(surf_selected->shader);
    		surf_selected->shader = Device.Shader.Create(lbShader->Caption.c_str(),surf_selected->textures);
			OnModified(Sender);
        }else{
            surf_selected->shader = Device.Shader.Create(lbShader->Caption.c_str(),surf_selected->textures);
            OnModified(Sender);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tvSurfacesItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
	if (Item==tvSurfaces->Selected) 	return;
    tx_selected 					= 0;
    surf_selected 					= 0;
    paSurface->Visible 				= Item->Level==1;
	paTexture->Visible 				= Item->Level==2;
    switch (Item->Level){
    case 0:
        paImage->Repaint();
    break;
    case 1:{
    	CEditableObject* O			= (CEditableObject*)Item->Parent->Data;
    	st_Surface* surf			= (st_Surface*)(Item->Data);
        lbSurfFaces->Caption 		= O->GetSurfFaceCount(surf->name);
        lbSurfSideFlag->Caption 	= (surf->sideflag)?"yes":"no";
        lbShader->Caption			= surf->shader->shader->cName;
        lbTexture->Caption			= ListToSequence(surf->textures);
        // FVF
        lbSurfFVF_XYZ->Caption		= (surf->dwFVF&D3DFVF_XYZ)?"yes":"no";
        lbSurfFVF_Normal->Caption  	= (surf->dwFVF&D3DFVF_NORMAL)?"yes":"no";
        lbSurfFVF_Diffuse->Caption	= (surf->dwFVF&D3DFVF_DIFFUSE)?"yes":"no";
        lbSurfFVF_Specular->Caption	= (surf->dwFVF&D3DFVF_SPECULAR)?"yes":"no";
        lbSurfFVF_XYZRHW->Caption	= (surf->dwFVF&D3DFVF_XYZRHW)?"yes":"no";
		lbSurfFVF_TCCount->Caption 	= ((surf->dwFVF&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
		surf_selected 				= surf;
    }break;
    case 2:
        tx_selected = Device.Shader.FindTexture(Item->Text.c_str());
        if (tx_selected){
            lbWidth->Caption = tx_selected->width();
            lbHeight->Caption = tx_selected->height();
            lbAlpha->Caption = (tx_selected->alpha())?"present":"absent";
            if (tx_selected->width()!=tx_selected->height()) paImage->Repaint();
            imPaint(Sender);
        }else{
        	ELog.DlgMsg(mtInformation,"Texture '%s' not used in current material.",Item->Text.c_str());
        }
    break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::imPaint(TObject *Sender)
{
    if (tx_selected){
        RECT r; r.left = 1; r.top = 1;
        float w, h;
        w = tx_selected->width();
        h = tx_selected->height();
        if (w>h){
        	r.right = im->Width; r.bottom = h/w*im->Height;
        }else{
        	r.right = w/h*im->Width; r.bottom = im->Height;}
		tx_selected->StretchThumbnail(paImage->Handle, &r);
    }
}

void __fastcall TfrmPropertiesObject::cbSurfSideFlagClick(TObject *Sender)
{
	TCheckBox* cb = dynamic_cast<TCheckBox*>(Sender);
    cb->State = TCheckBoxState(!cb->Checked);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ExtBtn1Click(TObject *Sender)
{
	tvSurfaces->FullExpand();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ExtBtn2Click(TObject *Sender)
{
	tvSurfaces->FullCollapse();
}
//---------------------------------------------------------------------------


