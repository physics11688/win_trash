# win_trash

Windows用のtrashです.

WSLがあると言っても,まあ一応ってことです.

でも本当は `<windows.h>` を使ってみるために昔遊びで書いただけです.

PowerShellを使ってるなら $profileにエイリアスを設定したほうがいいでしょう.

<br>

```pwsh
Set-Alias rm "$HOME\local\bin\trash.exe"
```

<br>

## Requirements

1. [Mingw-w64](https://www.mingw-w64.org/)をインストール済み
2. gccにパスが通っている

<br>

## Install

```bash
> git clone git@github.com:physics11688/win_trash.git

# ↑ が出来ない人は
> git clone https://github.com/physics11688/win_trash.git

> cd win_trash

# ビルド
> gcc -std=c11 -Wall -Wextra -fexec-charset=cp932 trash.c -o $HOME\local\bin\trash.exe

# 設置場所
> mkdir -p $HOME\local\bin

# バイナリの移動
> Move-Item -force .\trash.exe "$HOME\local\bin"

# profileの確認
> Test-Path $profile # Trueなら次へ進む. Falseなら→ New-Item -path $profile -type file -force

# エイリアスの設定
> echo 'Set-Alias rm "$HOME\local\bin\trash.exe"' >> $profile

# profileの読み込み
> . $profile

```

<br>

## Usage

```bash
> rm -h

---- ファイルをごみ箱に送るコマンド ----

基本的な使い方: trash.exe [ファイル]

オプション:

   -h  trashの説明とオプションを表示

   -v　ゴミ箱に送ったファイルのファイル名を表示

   -l  ゴミ箱GUI表示

   -e  ゴミ箱を空に

```
