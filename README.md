まだ途中です^^
# Library
Win32APIや、DirectX11を使用したライブラリです。

## Requirement
- Windows Vista 以降のバージョン
- Visual Studio 2022(Mingwなどのコンパイラーはテストしていません)
- TagLib v2.2.1
- opusfile v0.12
- opus v1.5.2
- libvorbis v1.3.7
- libogg

## Description
- このライブラリではワイド文字を使用しており、マルチバイトは直接サポートしていません。<br>Convert名前空間の関数からワイド文字に変換してください。
- このライブラリでは、C++20の機能を使用しています。<br>C++20以外のバージョンでのコンパイルは対応していません。

## Trouble Shooting
- Q.例外でたよ！　A.原因と解決法

<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
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
- Text(staticのみ)
### Image
- FIlter
- - GrayScale
- - Mosaic(使えない)
- - Invert

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
