#ifndef UI_MainExternH
#define UI_MainExternH

#include "UI_MainCommand.h"

enum {
	COMMAND_EXTFIRST_EXT = COMMAND_MAIN_LAST-1,

	COMMAND_CHANGE_TARGET,

	COMMAND_SHOW_OBJECTLIST,

	COMMAND_REFRESH_SOUND_ENVS,
    COMMAND_REFRESH_SOUND_ENV_GEOMETRY,
    
    COMMAND_LIBRARY_EDITOR,
    COMMAND_LANIM_EDITOR,
    COMMAND_FILE_MENU,
	COMMAND_CLEAR_COMPILER_ERROR,
    COMMAND_IMPORT_COMPILER_ERROR,
	COMMAND_VALIDATE_SCENE,
    COMMAND_REFRESH_LIBRARY,
    COMMAND_RELOAD_OBJECTS,

	COMMAND_CUT,
	COMMAND_COPY,
	COMMAND_PASTE,

	COMMAND_SCENE_SUMMARY,

	COMMAND_OPTIONS,
	COMMAND_BUILD,

	COMMAND_MAKE_GAME,
    COMMAND_MAKE_DETAILS,
	COMMAND_MAKE_HOM,
	COMMAND_MAKE_SKYDOME,

	COMMAND_INVERT_SELECTION_ALL,
	COMMAND_SELECT_ALL,
	COMMAND_DESELECT_ALL,
	COMMAND_DELETE_SELECTION,
	COMMAND_HIDE_UNSEL,
	COMMAND_HIDE_SEL,
	COMMAND_HIDE_ALL,
    COMMAND_LOCK_ALL,
    COMMAND_LOCK_SEL,
    COMMAND_LOCK_UNSEL,
    COMMAND_RESET_ANIMATION,
    COMMAND_SET_SNAP_OBJECTS,
    COMMAND_ADD_SNAP_OBJECTS,
    COMMAND_CLEAR_SNAP_OBJECTS,

    COMMAND_LOAD_FIRSTRECENT,

    COMMAND_MUTE_SOUND, // p0 - on/off
};
//------------------------------------------------------------------------------

#endif //UI_MainCommandH



