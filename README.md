# prog1702
A simple programmer using Raspberry Pi Zero for Intel 1702A vintage EPROM

This document is written in Japanese; please use Google or DeepL to translate.

Intelの古いEPROM 1702A用のプログラマです．書き込み機能に特化することにより，かなり簡単に作ることができました．読み込みはRetro Chip Tester (https://8bit-museum.de/rct) を使いました．

製作記的なものをブログにまとめました。(https://blog.goo.ne.jp/tk-80/e/27f34cfac8f6f955ffc4693c3acb4028)

## 電源について
- +12Vと-47Vの電源が必要です．私は+12Vと+47Vの絶縁型安定化電源を使い，+12Vの+側を+12V，+12Vの-側と+47Vの+側を接続してGND(0V), +47V電源の-側を-47Vとして使いました．
- Raspberry Pi ZeroのGNDは上記の-47Vに接続するので，GNDは絶縁されていることが必須です．私はラズパイ用の+5V電源はモバイルバッテリーを使用しました．
- 1702Aのデータシートには，+12Vの電源は100mAに電流制限するようにとあります．(が，特に何もしなくても電流はほとんど流れていない(10mA以下)ようでした．)

## Raspberry Piについて
- 書き込みパルス発生時に割り込み禁止にする必要があります．本プログラムはRaspberry Pi Zero専用に書かれており，RasPi 4では正常に動きません．

## 使い方
- 割り込み禁止にするため，sudoでルート権限にして起動します
- 256バイトのバイナリファイルをそのまま書き込みます．他のフォーマットには対応していません．
```
cd src
sodo ./prog1702 sample.bin
```
