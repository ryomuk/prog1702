# prog1702
A simple programmer using Raspberry Pi Zero for Intel 1702A vintage EPROM

This document is written in Japanese; please use Google or DeepL to translate.

Intelの古いEPROM 1702A用のプログラマです．書き込み機能に特化することにより，かなり簡単に作ることができました．読み込みはRetro Chip Tester (https://8bit-museum.de/sonstiges/hardware-projekte/hardware-projekte-chip-tester-english/) を使うと便利です．

## 電源について
- +12Vと-48Vの電源が必要です．私は+12Vと+48Vの絶縁型安定化電源を使い，+12Vの+側を+12V，+12Vの-側と+48Vの+側を接続してGND(0V), +48V電源の-側を-48Vとして使いました．
- Raspberry Pi ZeroのGNDは上記の-48Vに接続するので，GNDは絶縁されていることが必須です．私はラズパイ用の+5V電源はモバイルバッテリーを使用しました．
