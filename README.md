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
