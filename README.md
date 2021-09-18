# 👉 Deprecated! Please use [oDFe](https://github.com/jbustos22/odfe) 👈
This repository is deprecated. Please use [oDFe](https://github.com/jbustos22/odfe), as it:
- Also renders slick surfaces
- Improves memory usage and performance (ie. culling)
- Is actively maintained
- It's made to be actually used by people

# bxDFe
A proof-of-concept fork of Quake3e to primarily display triggers and invisible clips due to requests.

<img src="https://i.imgur.com/OqA5g6S.jpg" width="350px" style="display:inline-block;"/> <img src="https://i.imgur.com/Js3fTfc.jpg" width="350px" style="display:inline-block;"/>

The brush information for rendering triggers/clips was not readily available in the bsp. So, the information is recreated at load time by computing plane intersections and resolving the faces of the brush. These faces are then rendered each frame by the engine.
Most of the code for recreating and rendering is located at `code/client/cl_tc_vis.c`

## Compiling
Compiling is as simple as cloning the repository and running `make` (might have to use mingw on Windows).

**Note:** cmake is not supported at this time.

## Running
Place `pk3s/*.pk3` into your <installation directory>/baseq3 folder.

After compiling, copy and paste the resulting executable from the build folder to your Quake 3 installation directory and then run it.

Alternatively, you can invoke the executable from the build directory with the argument: `+set fs_homepath "install path"`

## Settings
To view clips, write `/bxdfe_clips_draw 1` in the console.

To view triggers, write `/bxdfe_triggers_draw 1` in the console.

## Known issues
- Patch brush clips/triggers are not displayed:
  - This can be easily solved by stopping `tr_bsp.c` from `SF_SKIP`ing invisible patch brushes
  - However, since the renderer cannot distinguish clips and triggers, it cannot color the patch properly
  - Furthermore, at the time patches are rendered OpenGL has back face culling enabled
  - So only one side of the patch will render even if this is done, unless culling is temporarily disabled
- Using brush models with custom entity fields are not fully supported:
  - Currently, the location field is supported (translation)
  - Fields like `angle/angles` (rotation) are not currently supported, but are easy to add
  - A vast majority of maps don't use this "feature", so this is not really a problem
- Brushes that use an invisible/fully transparent shader and/or don't use `CONTENTS_PLAYERCLIP` flag are not supported
- If a trigger/clip does not match the above issues and still fails to render, it is likely due to a exceeding `r_maxpolys`
  - As a temporary workaround increase the `r_maxpolys` cvar value and try to load the map again
  - To fix this problem, another method instead of `AddPolyToScene` should be used
- Z-fighting
  - It might be possible to fix many cases by using breadsticks' idea of slightly moving each face inwards.

## Thanks
- breadsticks
- Developers behind quake3e and Q3A
