# <img src="https://github.com/Sudo-Rahman/Iridium/blob/main/ressources/Iridium.png" width="50" height="50" align="center" /> Iridium

Graphical application for managing remote storage services such as Google Drive, Dropbox, OneDrive, etc. Using the Qt framework and C++, rclone and Boost libraries.
<br>

## Technologies

- [Qt](https://www.qt.io/)
- [C++](https://isocpp.org/)
- [CMake](https://cmake.org/)
- [Boost](https://www.boost.org/)
- [rclone](https://rclone.org/)

## Features

- Add remote :
    - [x] Google Drive
    - [x] Local
    - [x] One Drive
    - [x] Dropbox
    - [x] FTP
    - [x] SFTP
    - [x] Mega
    - [x] OpenDrive
    - [ ] SMB
    - [ ] PCloud
    - [ ] ...
- Manage remote :
  - [x] Copy
  - [x] Paste
  - [x] Rename
  - [x] Delete
  - [x] Create new folder
  - [x] View file properties
  - [x] View Transfer progress
  - [ ] Search
  - [ ] Sort
  - [ ] sync
  - [ ] ...

<br>

## Visuals

### Main window

<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen1.png" width="500" height="auto" align="center" />
<br>
<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen2.png" width="500" height="auto" align="center" />
<br>
<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen4.png" width="500" height="auto" align="center" />
<br>
<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen5.png" width="500" height="auto" align="center" />
<br>

### Add remote window
<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen3.png" width="500" height="auto" align="center" />
<br>

### Windows
<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen Windows.png" width="500" height="auto" align="center" />

## Installation

### Dependencies

- [Qt](https://www.qt.io/)
- [CMake](https://cmake.org/)
- [Boost](https://www.boost.org/)

### Compilation

```bash
git clone https://github.com/Sudo-Rahman/Iridium.git
cd Iridium
```
- Install rclone binary and add it to Iridium directory (see [rclone download page](https://rclone.org/downloads/))
- Set Cmake prefix path to Qt installation directory

```bash
mkdir build
cd build
cmake ..
make
```

<br>

### Authors

- [Sudo-Rahman](https://github.com/Sudo-Rahman)

### Additional Resources

- [Fluent icons](https://github.com/vinceliuice/Fluent-icon-theme)

### Licence

- [GPL V3](https://www.gnu.org/licenses/gpl-3.0.en.html)