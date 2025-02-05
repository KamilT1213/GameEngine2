#include "sound/soundManager.hpp"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
void SoundManager::playSound(const char* SoundPath) {
	PlaySound(TEXT(SoundPath), NULL, SND_FILENAME | SND_ASYNC);
}
