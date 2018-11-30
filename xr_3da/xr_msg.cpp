#include "stdafx.h"
#include "x_ray.h"
#include "xr_ioconsole.h"
#include "xr_creator.h"
#include "CameraManager.h"
#include "fdemorecord.h"
#include "fdemoplay.h"
#include "xr_list.h"
#include "xr_msg.h"

CList <xr_message_struct> xr_message_stack;

void PutMessage(xr_message m, DWORD p1, DWORD p2) {
	xr_message_struct s;

	s.msg=m;
	s.param1=p1;
	s.param2=p2;

	xr_message_stack.push_back(s);
}

extern void AddOne(char *s);

extern BOOL StartGame(DWORD dwLevel);

BOOL ProcessMessages(void) {
BOOL needcontinue=true;

	for (DWORD i=0; i<xr_message_stack.size(); i++) {
		switch (xr_message_stack[i].msg) {
		case msg_level_load:
			StartGame(xr_message_stack[i].param1);
			break;
		case msg_level_unload:
			if (pCreator) {
				Console.Hide	();
				DEL_INSTANCE	(pCreator);
				Console.Show	();
			}
			break;
		case msg_real_quit:
			PostQuitMessage					( 0 );
			needcontinue					=false;
			break;
		case msg_quit:
			PutMessage						(msg_real_quit);
			break;
		default:
			break;
		}
	}
	xr_message_stack.clear();
	return needcontinue;
}
