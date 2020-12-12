#pragma once

struct xr_message_struct {
	xr_message	msg;
	DWORD		param1;
	DWORD		param2;
};

void PutMessage(xr_message MSG, DWORD param1=0, DWORD param2=0);
BOOL ProcessMessages(void); // returns continue or not cycle, true=continue
