#ifndef UI_MainCommandH
#define UI_MainCommandH

enum ECommands{
	COMMAND_INITIALIZE,
	COMMAND_DESTROY,
	COMMAND_QUIT,
	COMMAND_EXIT,
	COMMAND_CLEAR,
	COMMAND_LOAD,
	COMMAND_RELOAD,
	COMMAND_SAVE,
	COMMAND_SAVEAS,
	COMMAND_BUILD,
	COMMAND_OPTIONS,
    COMMAND_FILE_MENU,
    COMMAND_LOAD_FIRSTRECENT,

    COMMAND_CHECK_MODIFIED,
	COMMAND_VALIDATE_SCENE,
	COMMAND_REFRESH_TEXTURES, 
	COMMAND_UNLOAD_TEXTURES,
	COMMAND_RELOAD_OBJECTS,
	COMMAND_REFRESH_LIBRARY,
	COMMAND_EDITOR_PREF,
	COMMAND_OBJECT_LIST,
	COMMAND_RENDER_FOCUS,
	COMMAND_UPDATE_CAPTION,
	COMMAND_BREAK_LAST_OPERATION,
	COMMAND_RESET_ANIMATION,
	COMMAND_MAKE_GAME,
	COMMAND_MAKE_DETAILS,
	COMMAND_LIBRARY_EDITOR,
	COMMAND_IMAGE_EDITOR,
	COMMAND_LANIM_EDITOR,
	COMMAND_CHECK_TEXTURES,
	COMMAND_UPDATE_TOOLBAR,

	COMMAND_CUT,
	COMMAND_COPY,
	COMMAND_PASTE,
	COMMAND_UNDO,
	COMMAND_REDO,

	COMMAND_INVERT_SELECTION_ALL,
	COMMAND_SELECT_ALL,
	COMMAND_DESELECT_ALL,

	COMMAND_DELETE_SELECTION,
	COMMAND_SHOWPROPERTIES,
	COMMAND_SHOWCONTEXTMENU,
	COMMAND_HIDE_ALL,
	COMMAND_HIDE_SEL,
	COMMAND_HIDE_UNSEL,
	COMMAND_ZOOM_EXTENTS,				// p1 -> selected/all (true/false)
	COMMAND_SET_SNAP_OBJECTS,
	COMMAND_ADD_SNAP_OBJECTS,
	COMMAND_CLEAR_SNAP_OBJECTS,
	COMMAND_UPDATE_GRID,
    COMMAND_GRID_NUMBER_OF_SLOTS,		// p1 <> (true,false)
    COMMAND_GRID_SLOT_SIZE,				// p1 <> (true,false)

	COMMAND_CHANGE_TARGET,           	// p1 -> target
	COMMAND_CHANGE_ACTION,           	// p1 -> action
	COMMAND_CHANGE_AXIS,             	// p1 -> enum EAxis
	COMMAND_CHANGE_SNAP,

	COMMAND_SET_NUMERIC_POSITION,
	COMMAND_SET_NUMERIC_ROTATION,
	COMMAND_SET_NUMERIC_SCALE,

	COMMAND_LOCK_ALL,
	COMMAND_LOCK_SEL,
	COMMAND_LOCK_UNSEL
};
//------------------------------------------------------------------------------

#endif //UI_MainCommandH



