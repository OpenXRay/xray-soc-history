#include "stdafx.h"
#pragma hdrstop

#include "TopBar.h"
#include "UI_ToolsCustom.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"
TfraTopBar *fraTopBar;
//---------------------------------------------------------------------------
__fastcall TfraTopBar::TfraTopBar(TComponent* Owner)
        : TFrame(Owner)
{
    ebActionSelect->Tag     = etaSelect;
    ebActionAdd->Tag        = etaAdd;
    ebActionMove->Tag       = etaMove;
    ebActionRotate->Tag     = etaRotate;
    ebActionScale->Tag      = etaScale;

    ebAxisX->Tag			= etAxisX;
    ebAxisY->Tag			= etAxisY;
    ebAxisZ->Tag			= etAxisZ;
    ebAxisZX->Tag			= etAxisZX;
    
	ebCSParent->Tag			= etfCSParent;
	ebNUScale->Tag			= etfNUScale;
	ebNormalAlign->Tag		= etfNormalAlign;
	ebGSnap->Tag			= etfGSnap;
	ebOSnap->Tag			= etfOSnap;
	ebMTSnap->Tag			= etfMTSnap;
	ebVSnap->Tag			= etfVSnap;
	ebASnap->Tag			= etfASnap;
	ebMSnap->Tag			= etfMSnap;

    DEFINE_INI(fsStorage);
}
//---------------------------------------------------------------------------

void TfraTopBar::OnTimer(){
    for (int j=0; j<ComponentCount; j++){
        TComponent* temp = Components[j];
        if (dynamic_cast<TExtBtn *>(temp) != NULL)
            ((TExtBtn*)temp)->UpdateMouseInControl();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ebEditUndoClick(TObject *Sender)
{
 	UI->Command( COMMAND_UNDO );
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ebEditRedoClick(TObject *Sender)
{
 	UI->Command( COMMAND_REDO );
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ActionClick(TObject *Sender)
{
    TExtBtn* btn=dynamic_cast<TExtBtn*>(Sender);
    VERIFY(btn);
    UI->Command(COMMAND_CHANGE_ACTION, btn->Tag);
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ebAxisClick(TObject *Sender)
{
    TExtBtn* btn=dynamic_cast<TExtBtn*>(Sender);
    VERIFY(btn);
    UI->Command(COMMAND_CHANGE_AXIS, btn->Tag);
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ebSettingsClick(TObject *Sender)
{
    TExtBtn* btn=dynamic_cast<TExtBtn*>(Sender);
    VERIFY(btn);
    UI->Command(COMMAND_CHANGE_SETTINGS, btn->Tag, btn->Down);
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ebViewClick(TObject *Sender)
{
    TExtBtn* btn=dynamic_cast<TExtBtn*>(Sender);
    if (btn){
        if (btn==ebViewFront)   Device.m_Camera.ViewFront();
        if (btn==ebViewBack)    Device.m_Camera.ViewBack();
        if (btn==ebViewLeft)    Device.m_Camera.ViewLeft();
        if (btn==ebViewRight)   Device.m_Camera.ViewRight();
        if (btn==ebViewTop)     Device.m_Camera.ViewTop();
        if (btn==ebViewBottom)  Device.m_Camera.ViewBottom();
        if (btn==ebViewReset)  	Device.m_Camera.ViewReset();
    }
    UI->RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::RefreshBar()
{
    TExtBtn* btn		= 0;
	//actions
    switch(Tools->GetAction()){
    case etaSelect: 	btn=ebActionSelect; 	break;
    case etaAdd:    	btn=ebActionAdd; 		break;
    case etaMove:   	btn=ebActionMove; 		break;
    case etaRotate: 	btn=ebActionRotate; 	break;
    case etaScale:		btn=ebActionScale;		break;
    default: THROW;
    }
	btn->Down 			= true;
    // axis
	switch (Tools->GetAxis()){
    case etAxisX: 		btn=ebAxisX; 	break;
    case etAxisY: 		btn=ebAxisY; 	break;
    case etAxisZ: 		btn=ebAxisZ; 	break;
    case etAxisZX: 		btn=ebAxisZX; 	break;
    default: THROW;
    }
	btn->Down 			= true;
    // settings
	ebCSParent->Down	= Tools->GetSettings(etfCSParent);
	ebNUScale->Down		= Tools->GetSettings(etfNUScale);
	ebNormalAlign->Down	= Tools->GetSettings(etfNormalAlign);
	ebGSnap->Down		= Tools->GetSettings(etfGSnap);
	ebOSnap->Down		= Tools->GetSettings(etfOSnap);
	ebMTSnap->Down		= Tools->GetSettings(etfMTSnap);
	ebVSnap->Down		= Tools->GetSettings(etfVSnap);
	ebASnap->Down		= Tools->GetSettings(etfASnap);
	ebMSnap->Down		= Tools->GetSettings(etfMSnap);
    // redraw scene
    UI->RedrawScene		();
}
//---------------------------------------------------------------------------


void __fastcall TfraTopBar::ebZoomExtentsClick(TObject *Sender)
{
 	UI->Command( COMMAND_ZOOM_EXTENTS, FALSE );
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ebZoomExtentsSelectedClick(TObject *Sender)
{
 	UI->Command( COMMAND_ZOOM_EXTENTS, TRUE );
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ebCameraStyleClick(TObject *Sender)
{
	Device.m_Camera.SetStyle((ECameraStyle)((TExtBtn*)Sender)->Tag);
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::fsStorageRestorePlacement(TObject *Sender)
{
	if (ebCameraPlane->Down) 			Device.m_Camera.SetStyle(csPlaneMove);
	else if (ebCameraArcBall->Down) 	Device.m_Camera.SetStyle(cs3DArcBall);
	else if (ebCameraFly->Down) 		Device.m_Camera.SetStyle(csFreeFly);
    UI->RedrawScene();
}
//---------------------------------------------------------------------------






