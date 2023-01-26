# ScreenFlip

A public software that can remotely change soe Windows settings. Use on you test machine only.

# Disclaimer

Attention! This program can damage this computer! Run only on your own computer! Run only on a test system! Use for educational, research and demonstration purposes only! It is forbidden to use this program on a computer if you do not have a direct authorization to run this program from the owner of the computer!

By using this program, you agree that this program can perform any action with this computer, all information stored on it, and all devices connected to it, including, but not limited to the following actions:
- Changing the settings of the monitor or other display;
- Loading and saving data on local disks from the Internet;
- Change desktop wallpaper;
- Changing the settings of the computer mouse;
- Changing the status of open windows;
- Reading and processing information from local disks or external storage devices;
- Reading and processing properties of open windows;
- Connection to external servers via the Internet.

By using this program, you agree that this program can connect to the remote servers of the author of this program, which are located in Perm, Russia, and that this computer can receive remote commands from the server, and from everyone connected to this server there will be remote access to your computer as well.

The author of this program is not responsible for any harm caused by this program to this computer, other computers, individuals and legal entities, destruction, copying, modification, blocking or any other actions performed by this program in relation to your data.

By using this program, you agree to all of the above conditions and give your sanction to all of the above and, in general, any actions with your computer, assign all responsibility for these actions solely to yourself, confirm that you are fully familiar with this agreement.

# Compatibility

This program is fully tested by me and seems to work on Windows 10 21H2 and Windows 7 Ultimate with Service Pack 1, but I assume that it will work on all Windows versions from Vista to 11.

# Usage

The config file for ScreenFlip should follow this specification:
- It has to have 'config.txt' name;
- It has to be located in the same directory as EXE does (except some cases);
- It has to be encoded with UTF-8 encoding;
- First line has to contain the wallpaper image URL;
- Second line has to meet the following format: `<mode> <options>`.
 
# Modes
 
ScreenFlip supports following modes:
`instant` changes Windows settings immediately after execution;
`mine <list> <of> <arguments>` detects if the user has open window, which name contains one of the specified options, then changes settings;
`remote <host> <port>` connects to remote TCP server and waits for the ASCII-encoded command `ok`, then changes settings.
  
# Build
  
Build with MSVC 19.29.30147 for x86, which comes with Visual Studio 2022 C/C++ package.
Put source files to MSVC directory and run this command:
`cl main.c sockets.c user32.lib`
You will get a `main.exe` file in MSVC directory.
