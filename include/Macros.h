#pragma once
#include<stdexcept>

//Nodiscard
#define RETURN [[nodiscard]]
//final
#define SEALED final
//Null Check
#define AST_NULL(ptr) {if(ptr==nullptr){throw std::runtime_error("ポインタがnullptrでした。");}}
//XAudio2 Loop Infinite
#define INFINITE_LOOP XAUDIO2_LOOP_INFINITE
