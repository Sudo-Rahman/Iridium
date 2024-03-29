# <img src="https://github.com/Sudo-Rahman/Iridium/blob/main/ressources/Iridium.png" width="50" height="50" align="center" /> Iridium

Graphical application for managing remote storage services such as Google Drive, Dropbox, OneDrive, etc. Written in C++, using Qt framework, boost library and rclone binary.
<br>

## Technologies

- [Qt](https://www.qt.io/)
- [C++](https://isocpp.org/)
- [CMake](https://cmake.org/)
- [Boost](https://www.boost.org/)
- [Rclone](https://rclone.org/)

[//]: # (- [Libcurl]&#40;https://curl.se/libcurl/&#41;)
[//]: # (- [LibZip]&#40;https://libzip.org/&#41;)

## Features

- Remote :
    - [x] Google Drive
    - [x] Local
    - [x] One Drive
    - [x] Dropbox
    - [x] FTP
    - [x] SFTP
    - [x] Mega
    - [x] OpenDrive
    - [x] SMB
    - [x] PCloud
    - [x] Box
    - [x] ...
- Manage remote :
  - [x] Copy
  - [x] Paste
  - [x] Rename
  - [x] Delete
  - [x] Create new folder
  - [x] View file properties
  - [x] View Transfer progress
  - [x] Drag and drop
  - [x] Search
  - [x] Sort by name, size, date, type
  - [x] Tree view on folder
  - [ ] sync
  - [ ] ...

<br>

## Visuals

<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen1.png" width="500" height="auto" align="center" />
<br>
<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen2.png" width="500" height="auto" align="center" />
<br>
<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen3.png" width="500" height="auto" align="center" />
<br>
<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen4.png" width="500" height="auto" align="center" />
<br>
<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen5.png" width="500" height="auto" align="center" />
<br>
<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen6.png" width="500" height="auto" align="center" />

## Installation

### Dependencies

- [Qt](https://www.qt.io/)
- [CMake](https://cmake.org/)
- [Boost](https://www.boost.org/)

[//]: # (- [Libcurl]&#40;https://curl.se/libcurl/&#41;)
[//]: # (- [LibZip]&#40;https://libzip.org/&#41;)

### Compilation

```bash
git clone https://github.com/Sudo-Rahman/Iridium.git
cd Iridium
```
- Set Cmake prefix path to Qt installation directory
- Set Cmake boost path to Boost installation directory

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
