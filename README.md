# hobby_amazfit_bip_ext_dnd V2.0
Extra functions for DND screen (BipOS v5.5+)

![image](https://user-images.githubusercontent.com/7702203/113303962-def85880-9301-11eb-86a4-54625ba46e7c.png)

**Functions:**
- Switch DND modes: OFF (grey) => ON (green) => AUTO (blue with zz icon)
- Find my phone
- Torch (flash light): swipe left to access (This app is built-in)
- Music control: swipe right to access (This app is built-in)
- Two customizable buttons (can change app to launch and button icon)

**Note:** Flash light and music control functionalities are built into the program. You don't need to install them separately.

# HOW TO INSTALL

This app allows you to have 2 customizable buttons to launch apps. By default:
- Button A launchs app index 4 in your list
- Button B launchs app index 5 in your list

### How to change icon

You can replace res ID **0000** (big zero digit in orange) and **0001** (big zero digit in orange) in your watch's res file.  
The reason that I choose these two images belong to one of the built-in watch face. Most of the time, we use custom watch faces.

![image](https://user-images.githubusercontent.com/7702203/113303761-b1abaa80-9301-11eb-9aae-98b5e953c86d.png)

**Useful info**: You can file photoshop files under [asset_extdnd](https://github.com/ngxson/hobby_amazfit_bip_ext_dnd/tree/main/asset_extdnd) folder.

### How to change app

Buttons can launch app at index 4, 5.  
It is because these app **cannot** be easily reached. It will be useless to launch app index 1, 2 because it is easier to be reached.

**Attention**: You must also count HIDDEN APPS like alarm_edit. For example, your app list looks like this:

```
0. alarm_edit (this app is hidden)
----- page 1/2 -----
1. calendar
2. calculator
3. fitness timer
----- page 2/2 -----
4. animation for main menu (this app is hidden)
5. notes
6. HR chart
7. tetris
```

You want to assign **calendar to button A** and **calculator to button B**:

```
0. alarm_edit (this app is hidden)
----- page 1/2 -----
1. notes
2. HR chart
3. fitness timer
----- page 2/2 -----
4. calendar     <= Button A
5. calculator   <= Button B
6. animation for main menu (this app is hidden)
7. tetris
```

# Author
Github: [@ngxson](https://github.com/ngxson)  
Website: [ngxson.com](https://ngxson.com)  

This program is based on [@MNVolkov](https://github.com/MNVolkov)'s works:
- [BipOS](https://myamazfit.ru/threads/bip-mnvolkov-bipos-en.1087/) by MNVolkov
- [simpleDND](https://github.com/MNVolkov/simpleDND) by MNVolkov
- [Music_Control](https://github.com/MNVolkov/Music_Control) by MNVolkov
- [Flashlight](https://github.com/MNVolkov/Flashlight) by MNVolkov
