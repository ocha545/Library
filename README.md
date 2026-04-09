まだ途中です^^
# Library
Win32APIや、DirectX11を使用したライブラリです。

## Requirement
- Windows Vista 以降のバージョン
- Visual Studio 2022(Mingwなどのコンパイラーはテストしていません)

## Usage
ライブラリを任意のディレクトリにクローンし、ReleaseとDebugのどちらもビルドしてください。
```C++
#include"include/Window.h"
using namespace Win32;

コード例...
```

## Trouble Shooting
- Q.例外でたよ！　A.原因と解決法

<br>
<br>
<br>






てすと！

# **ある機能**
# Window (Win32API)
## Class
- Window
## Enum
- Position
- Result
- Button
- Icon
# Graphics (DirectX11)
## Class
- Rect
- Circle
- Texture
- Image
- Text(staticのみ)

# Audio (XAudio2)
## Class
- Audio
- WAVE
- MP3
- FLAC
- OGG
- OPUS
## Format
- Wave  (dr_wav)
- MP3   (dr_mp3)
- Flac  (dr_flac)
- ogg   (vorbis)
- opus  (opusfile)
## Struct
- AudioMetaData
- AudioInfo
## Enum
- AudioFormat

# Device
## Class
- Input(Keyboard & mouse)
## Enum
- Key
