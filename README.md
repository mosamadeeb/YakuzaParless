# YakuzaParless
ASI hook that redirects file paths in Yakuza series PC games to allow loading loose files outside of game archives.

# Installation
Refer to [Shin Ryu Mod Manager](https://github.com/SRMM-Studio/ShinRyuModManager), as this project is not intended to work on its own.

# Usage
Please check the [Shin Ryu Mod Manager wiki](https://github.com/SRMM-Studio/ShinRyuModManager/wiki).

# Current Limitations
While most of the files in the supported games can be redirected by Parless, there are some files that are loaded from other functions that are still unsupported. Future support for these files is expected. These files consist mostly of middleware audio and video containers.

The files can be summed up as the following:

For Yakuza 5 only:
```
ALL .cpk files
```

For all games:
```
ALL .usm files
```

# Building
Use [Premake](https://premake.github.io/) to generate the solution files.

# Credits
Original project by [SutandoTsukai181](https://github.com/SutandoTsukai181).

Based on [CookiePLMonster](https://github.com/CookiePLMonster)'s SilentPatches and [ModUtils](https://github.com/CookiePLMonster/ModUtils). Also using [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader).

Special thanks to [Violet](https://github.com/SamuraiOndo) for discovering a way to load unpacked PAR files, which made this project possible.

Thanks to [Timo654](https://github.com/Timo654), [Ret](https://github.com/Ret-HZ), and [metman98uk](https://github.com/metman98uk) for extensive testing during the early stages of this project.

Thanks to [Jhrino](https://github.com/Fronkln) for maintaining the project and adding new features.
