# <img src="https://github.com/Sudo-Rahman/Iridium/blob/main/resources/Iridium.png" width="50" height="50" align="center" /> Iridium

Graphical application for managing remote storage services such as Google Drive, Dropbox, OneDrive, etc. Written in C++, using Qt framework, boost library and rclone binary.
<br>

## Technologies

- [C++](https://isocpp.org/)
- [CMake](https://cmake.org/)
- [Qt](https://www.qt.io/)
- [Conan](https://conan.io/)
- [Boost](https://www.boost.org/)
- [Rclone](https://rclone.org/)
- [Libcurl](https://curl.se/libcurl/)
- [LibZip](https://libzip.org/)

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
    - [x] Alias
    - [x] Crypt
    - [x] ...
- Manage remote :
  - [x] Copy
  - [x] Paste
  - [x] Rename
  - [x] Delete
  - [x] Move
  - [x] Create new folder
  - [x] View file properties
  - [x] View Transfer progress
  - [x] Drag and drop
  - [x] Search
  - [x] Sort by name, size, date, type
  - [x] Tree view on folder
  - [x] sync

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
<br>
<img src="https://github.com/Sudo-Rahman/Iridium/blob/main/screen/screen6.png" width="500" height="auto" align="center" />

## Installation

### Dependencies

- [CMake](https://cmake.org/)
- [Qt](https://www.qt.io/)
- [Boost](https://www.boost.org/)
- [Libcurl](https://curl.se/libcurl/)
- [LibZip](https://libzip.org/)

### Compilation

#### MacOS/Linux
  - Install conan package manager for boost, libcurl, libzip dependencies.
  - Download and build [rclone_cpp library](https://github.com/Sudo-Rahman/rclone_cpp).
  - Download shared Qt library or build it from source.

    ```bash
    git clone https://github.com/Sudo-Rahman/Iridium.git
    cd Iridium
    conan install . --build=missing
    cmake --preset conan-release -DCMAKE_PREFIX_PATH="path/to/Qt/installation/cmake"
    cmake --build --parallel --preset conan-release
    ```

<br>

### Authors

- [Sudo-Rahman](https://github.com/Sudo-Rahman)

### Additional Resources

- [Fluent icons](https://github.com/vinceliuice/Fluent-icon-theme)

### Licence

- [GPL V3](https://www.gnu.org/licenses/gpl-3.0.en.html)
