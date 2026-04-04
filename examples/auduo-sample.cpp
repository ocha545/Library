#include"../include/Audio.h"
using namespace Win32;

int main()
{
	Audio audio{ L"music/Blue.mp3" };

	audio.Play();

	while (audio.State().BuffersQueued)
	{
	}
}