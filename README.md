# <img src="https://github.com/Sudo-Rahman/Iridium/blob/main/ressources/app.png" width="50" height="50" align="center" /> Iridium

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
    - [ ] One Drive
    - [ ] Dropbox
    - [ ] FTP
    - [x] SFTP
    - [ ] ...
- Manage remote :
  - [x] Copy
  - [x] Paste
  - [x] Rename
  - [x] Delete
  - [x] Create new folder
  - [x] View file properties
  - [x] View Transfer progress

<br>

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

- [LGPG](https://www.gnu.org/licenses/lgpl-3.0.fr.html)