# YakuzaParless
ASI hook that redirects file paths in Yakuza series PC games to allow loading loose files outside of game archives.

# Installation
Refer to [Ryu Mod Manager](https://github.com/SutandoTsukai181/RyuModManager), as this project is not intended to work on its own.

# Usage
Please check [Ryu Mod Manager wiki](https://github.com/SutandoTsukai181/RyuModManager/wiki).

# Current Limitations
While most of the files in the supported games can be redirected by Parless, there are some files that are loaded from other functions that are still unsupported. Future support for these files is expected. These files consist mostly of middleware audio and video containers. For Old Engine (Yakuza games released before Yakuza 6), this means audio/bgm and pre-rendered cutscenes. For Dragon Engine (Yakuza games not using Old Engine), a custom fix for audio was included, so only cutscenes are not supported.

The files can be summed up as the following:

For Old Engine only:
```
ALL .usm files
ALL .cpk files
ALL .hca and .adx files
```

For Dragon Engine only:
```
ALL .usm files
```

# Building
Use [Premake](https://premake.github.io/) to generate the solution files.

# Credits
Based on [CookiePLMonster](https://github.com/CookiePLMonster)'s SilentPatches and [ModUtils](https://github.com/CookiePLMonster/ModUtils). Also using [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader).

Special thanks to Violet (SpongeX#0305) from the [Yakuza Modding Community](https://discord.gg/WQutWJq) discord for discovering a way to load unpacked PAR files, which made this project possible.

Thanks to [Timo654](https://github.com/Timo654), [Capitán Retraso](https://github.com/CapitanRetraso), and metman98uk#1989 for extensive testing during the early stages of this project.
