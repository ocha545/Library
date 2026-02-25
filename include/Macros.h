#pragma once
#include<stdexcept>

//Nodiscard
#ifndef RETURN
#define RETURN [[nodiscard]]
#endif
//final
#ifndef SEALED
#define SEALED final
#endif
//Null Check
#ifndef AST_NULL
#define AST_NULL(ptr) {if(ptr==nullptr){throw std::runtime_error("ポインタがnullptrでした。");}}
#endif
//XAudio2 Loop Infinite
#ifndef INFINITE_LOOP
#define INFINITE_LOOP XAUDIO2_LOOP_INFINITE
#endif
