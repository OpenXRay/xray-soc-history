//---------------------------------------------------------------------------
#ifndef ItemListH
#define ItemListH
//---------------------------------------------------------------------------

#include "ElTree.hpp"
#include "ElTreeStdEditors.hpp"
#include "ElXPThemedControl.hpp"
#include "multi_edit.hpp"
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include "ElTreeAdvEdit.hpp"
#include <Mask.hpp>

#include "ItemListHelper.h"
#include "FolderLib.h"
#include "PropertiesListTypes.h"

class TItemList: public TForm, public IItemList
{
__published:	// IDE-managed Components
	TElTree *tvItems;
	TFormStorage 		*fsStorage;
	TMxPopupMenu *pmSystem;
	TMenuItem *ExpandAll1;
	TMenuItem *N1;
	TMenuItem *CollapseAll1;
	TMenuItem *N2;
	TMenuItem *miDrawThumbnails;
	TMxPopupMenu *pmItems;
	TElTreeInplaceAdvancedEdit *InplaceEdit;
	TMxPopupMenu *pmEdit;
	TMenuItem *MenuItem1;
	TMenuItem *MenuItem2;
	TMenuItem *MenuItem3;
	TMenuItem *MenuItem4;
	TMenuItem *MenuItem5;
	TMenuItem *N3;
	TMenuItem *RefreshForm1;
	TPanel *paStatus;
	TBevel *Bevel2;
	void __fastcall 	FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall 	tvItemsClick(TObject *Sender);
	void __fastcall 	tvItemsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall 	FormShow(TObject *Sender);
	void __fastcall 	tvItemsItemChange(TObject *Sender, TElTreeItem *Item, TItemChangeMode ItemChangeMode);
	void __fastcall 	CollapseAll1Click(TObject *Sender);
	void __fastcall 	ExpandAll1Click(TObject *Sender);
	void __fastcall 	miDrawThumbnailsClick(TObject *Sender);
	void __fastcall 	tvItemsMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall 	tvItemsMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall tvItemsAfterSelectionChange(TObject *Sender);
	void __fastcall tvItemsItemDraw(TObject *Sender, TElTreeItem *Item,
          TCanvas *Surface, TRect &R, int SectionIndex);
	void __fastcall InplaceEditValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall miCreateFolderClick(TObject *Sender);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall tvItemsDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall Delete1Click(TObject *Sender);
	void __fastcall InplaceEditAfterOperation(TObject *Sender, bool &Accepted,
          bool &DefaultConversion);
	void __fastcall tvItemsKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall tvItemsResize(TObject *Sender);
	void __fastcall RefreshForm1Click(TObject *Sender);
	void __fastcall tvItemsHeaderResize(TObject *Sender);
	void __fastcall tvItemsCompareItems(TObject *Sender, TElTreeItem *Item1,
          TElTreeItem *Item2, int &res);
private:	// User declarations
    AStringVec 			last_selected_items;

    ListItemsVec 		m_Items;
    void 				ClearParams				(TElTreeItem* node=0);

	void 				OutBOOL					(BOOL val, TCanvas* Surface, const TRect& R);
	void 				OutText					(LPCSTR text, TCanvas* Surface, TRect R, TGraphic* g=0, bool bArrow=false);
public:
    s32					iLocked;
    Flags32				m_Flags;
	// events
    TOnModifiedEvent  	OnModifiedEvent;
    TOnILItemsFocused	OnItemsFocusedEvent;
    TOnILItemFocused	OnItemFocusedEvent;
    TOnILCloseEvent	  	OnCloseEvent;
    CFolderHelper::TOnItemRename	OnItemRenameEvent;
    CFolderHelper::TOnItemRemove	OnItemRemoveEvent;
protected:
	// RT store
    struct SFolderStore{
    	bool expand;
    };
    DEFINE_MAP(AnsiString,SFolderStore,FolderStoreMap,FolderStorePairIt);
    FolderStoreMap		FolderStore;
protected:
    void __fastcall		RenameItem				(LPCSTR fn0, LPCSTR fn1, EItemType type);
public:		// User declarations
	__fastcall 	    	TItemList	       		(TComponent* Owner);

    void 			 	ShowListModal			();
    void 				ShowList				();
    void 				HideList				();
    void 				RefreshForm				();

    void 				DeselectAll				();
    virtual void 		SelectItem				(LPCSTR full_name, bool bVal, bool bLeaveSel, bool bExpand);
    virtual void 		AssignItems				(ListItemsVec& values, bool full_expand, bool full_sort=false);
    bool 				IsFocused				(){return tvItems->Focused();}

    virtual int  		GetSelected				(ElItemsVec& items);
    virtual int  		GetSelected				(LPCSTR pref, ListItemsVec& items, bool bOnlyObject);
    virtual TElTreeItem*GetSelected				(){R_ASSERT(!tvItems->MultiSelect); return (tvItems->MultiSelect)?0:tvItems->Selected;}
    virtual ListItem*	FindItem				(LPCSTR full_name);

    virtual void 		LockUpdating			(){ tvItems->IsUpdating = true; iLocked++; }
    virtual void 		UnlockUpdating			(){ tvItems->IsUpdating = false;iLocked--; VERIFY(iLocked>=0);}
    virtual bool		IsLocked				(){ return (iLocked>0); }

    virtual void		SetImages				(TImageList* image_list){tvItems->Images=image_list;}

    void				LoadSelection			(TFormStorage* storage);
    void				SaveSelection			(TFormStorage* storage);

    virtual void  		SaveParams				(TFormStorage* fs)
    {
//		fs->WriteInteger(AnsiString().sprintf("%s_column0_width",Name.c_str()),tvItems->HeaderSections->Item[0]->Width);
		fs->WriteInteger(AnsiString().sprintf("%s_draw_thm",Name.c_str()),miDrawThumbnails->Checked);
        SaveSelection							(fs);
    }
    virtual void  		LoadParams				(TFormStorage* fs)
    {
    	LoadSelection							(fs);
//		tvItems->HeaderSections->Item[0]->Width = fs->ReadInteger(AnsiString().sprintf("%s_column0_width",Name.c_str()),tvItems->HeaderSections->Item[0]->Width);
        miDrawThumbnails->Checked				= fs->ReadInteger(AnsiString().sprintf("%s_draw_thm",Name.c_str()),false);
        RefreshForm			();
    }

    virtual void 		RemoveSelItems			(CFolderHelper::TOnItemRemove on_remove=0);
    virtual void 		RenameSelItem			();
    virtual void		FireOnItemFocused		();

    virtual void		GetFolders				(RStrVec& folders);

    virtual void 		OnCreate				(LPCSTR title, TWinControl* parent, TAlign align, u32 flags);
    virtual void 		OnDestroy				();

    virtual void 		ClearList				();

	virtual void 		GenerateObjectName		(ref_str name, LPCSTR start_node, LPCSTR pref="object", bool num_first=false);

    virtual void		SetOnItemsFocusedEvent	(TOnILItemsFocused e)			{OnItemsFocusedEvent=e;}
    virtual void		SetOnCloseEvent			(TOnILCloseEvent e)				{OnCloseEvent=e;}
    virtual void		SetOnItemRenameEvent	(CFolderHelper::TOnItemRename e){OnItemRenameEvent=e;}
    virtual void		SetOnItemRemoveEvent	(CFolderHelper::TOnItemRemove e){OnItemRemoveEvent=e;}
    virtual void		SetOnModifiedEvent		(TOnModifiedEvent e)			{OnModifiedEvent=e;}
};
//---------------------------------------------------------------------------
#endif
