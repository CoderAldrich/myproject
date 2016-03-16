/***********************************************
 *                    MCI.h                    *
 ***********************************************/
#pragma once
#include "mmsystem.h"

class CMCI
{
private:
	MCIDEVICEID nDeviceID;

public:
	CMCI(void);
	~CMCI(void);
	BOOL Open(CString strSongPath);
	BOOL Play();
	BOOL Pause();
	BOOL Stop();
	BOOL Close();
	BOOL GetCurrentPos(DWORD &pos);
	BOOL SetVolume(int nVolumeValue);
};