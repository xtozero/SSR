# Screen Space Reflection

[![Build status](https://ci.appveyor.com/api/projects/status/5353852l11t3u3ka/branch/master?svg=true)](https://ci.appveyor.com/project/xtozero/ssr/branch/master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/a05014aa769e4410b66f6b49e0a1884d)](https://www.codacy.com/project/xtozero/SSR/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=xtozero/SSR&amp;utm_campaign=Badge_Grade_Dashboard)

## Key feature

- Screen space reflection
- Shadow mapping ( PSM, LiSPSM )
- Cascade shadow mapping
- Simple rigid body physics
- Atmospheric scattering ( Bruneton's Model )

## Prerequisite

- Windows 7 or later
- CMake
- Visual Studio 2017
- [DirectX SDK June 2010](https://www.microsoft.com/en-us/download/details.aspx?id=6812)

## How to build

1. Clone repository

2. Create 'Build' Folder in repository folder

3. Open command line

4. Move directory to 'Build' Folder

5. Type in command line

   ```
   cmake .. -G "Visual Studio 15 Win64"
   ```
   
6. Open 'Screen Space Reflection.sln' and Build

## Screenshot

![][01]

![][02]

![][03]

![][04]



[01]:./Screenshot/01.png
[02]:./Screenshot/02.gif
[03]: ./Screenshot/03.gif
[04]: ./Screenshot/04.png

