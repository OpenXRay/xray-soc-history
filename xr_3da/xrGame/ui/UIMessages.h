//=============================================================================
//  Filename:   UIMessages.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ������ ���� ��������� ���� ��������� � ���������� ���� ���������� ������� 
//	S.T.A.L.K.E.R. Shadow of chernobyl
//=============================================================================

#ifndef UI_MESSAGES_H_
#define UI_MESSAGES_H_

//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

enum EUIMessages
{
	// CUIWindow
	WINDOW_LBUTTON_DOWN = 0,
	WINDOW_RBUTTON_DOWN,
	WINDOW_LBUTTON_UP,
	WINDOW_RBUTTON_UP,
	WINDOW_MOUSE_MOVE,
	WINDOW_MOUSE_WHEEL_UP,
	WINDOW_MOUSE_WHEEL_DOWN,
	WINDOW_LBUTTON_DB_CLICK,
	WINDOW_KEY_PRESSED,
	WINDOW_KEY_RELEASED,
	WINDOW_MOUSE_CAPTURE_LOST, 
	WINDOW_KEYBOARD_CAPTURE_LOST,

	// CUIStatic
	STATIC_FOCUS_RECEIVED,
	STATIC_FOCUS_LOST,

	// CUIButton
	BUTTON_CLICKED,
	BUTTON_DOWN,

	// CUITabControl
	TAB_CHANGED,	

	// CUICheckButton
	CHECK_BUTTON_SET,
	CHECK_BUTTON_RESET, 

	// CUIRadioButton
	RADIOBUTTON_SET,

	// CUIdragDropItem
	DRAG_DROP_ITEM_DRAG,
	DRAG_DROP_ITEM_DROP, 
	DRAG_DROP_ITEM_MOVE,
	DRAG_DROP_ITEM_DB_CLICK,
	DRAG_DROP_ITEM_RBUTTON_CLICK,
	DRAG_DROP_REFRESH_ACTIVE_ITEM,
	DRAG_DROP_ITEM_SELECTED,
	DRAG_DROP_ITEM_PLACED,
	
	//DragDropList
	DRAG_DROP_LIST_SEL_CHANGED,

	// CUIScrollBox
	SCROLLBOX_MOVE,
	SCROLLBOX_STOP,

	// CUIScrollBar
	SCROLLBAR_VSCROLL,
	SCROLLBAR_HSCROLL,
	SCROLLBAR_NEEDUPDATE,

	// CUIScrollView
	CHILD_CHANGED_SIZE,

	// CUIListWnd
	LIST_ITEM_CLICKED,
	LIST_ITEM_SELECT,
	LIST_ITEM_UNSELECT,

	// CUIInteractiveItem
	INTERACTIVE_ITEM_CLICK,

	// UIPropertiesBox
	PROPERTY_CLICKED,

	// CUIMessageBox
	MESSAGE_BOX_OK_CLICKED,
	MESSAGE_BOX_YES_CLICKED,
	MESSAGE_BOX_NO_CLICKED,
	MESSAGE_BOX_CANCEL_CLICKED,

	// CUITalkDialogWnd
	TALK_DIALOG_TRADE_BUTTON_CLICKED,
	TALK_DIALOG_QUESTION_CLICKED,

	// CUIPdaWnd
	PDA_OPEN_ENCYCLOPEDIA_ARTICLE,
	PDA_ENCYCLOPEDIA_HAS_ARTICLE,
	// CUIPdaDialogWnd
	PDA_DIALOG_WND_BACK_BUTTON_CLICKED,
	PDA_DIALOG_WND_MESSAGE_BUTTON_CLICKED,

	// CUIPdaContactsWnd
	PDA_CONTACTS_WND_CONTACT_SELECTED,

	// CUITradeWnd
	TRADE_WND_CLOSED,

	// CUISleepWnd
	SLEEP_WND_PERFORM_BUTTON_CLICKED,

	// CUIOutfitSlot
	UNDRESS_OUTFIT,
	OUTFIT_RETURNED_BACK,

	// CUIArtifactMerger
	ARTEFACT_MERGER_CLOSE_BUTTON_CLICKED,
	ARTEFACT_MERGER_PERFORM_BUTTON_CLICKED,

	// CUIInventroyWnd
	INVENTORY_DROP_ACTION,
	INVENTORY_EAT_ACTION,
	INVENTORY_TO_BELT_ACTION, 
	INVENTORY_TO_SLOT_ACTION,
	INVENTORY_TO_BAG_ACTION,
	INVENTORY_ATTACH_ADDON, 
	INVENTORY_DETACH_SCOPE_ADDON,
	INVENTORY_DETACH_SILENCER_ADDON,
	INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON,
	INVENTORY_ACTIVATE_ARTEFACT_ACTION,
	INVENTORY_RELOAD_MAGAZINE,
	INVENTORY_UNLOAD_MAGAZINE,
	INVENTORY_SELL_ITEM,

	// BuyWeaponWnd
	XR_MENU_LEVEL_CHANGED,

	//CUIAnimationBase
    ANIMATION_STOPPED,
	EDIT_TEXT_CHANGED,
	MAP_SHOW_HINT,
	MAP_HIDE_HINT,
	MAP_SELECT_SPOT,
};

//////////////////////////////////////////////////////////////////////////

#endif //UI_MESSAGES_H_