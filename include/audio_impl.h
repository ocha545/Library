#pragma once
#pragma warning(push)
#pragma warning(disable: 6262)
#pragma warning(disable: 6385)
#pragma warning(disable: 6386)
#define DR_WAV_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#define DR_FLAC_IMPLEMENTATION
#include"third_party/dr_wav.h"
#include"third_party/dr_mp3.h"
#include"third_party/dr_flac.h"
#pragma warning(pop)
#include"third_party/vorbis/vorbisfile.h"
#include"third_party/opusfile/opusfile.h"