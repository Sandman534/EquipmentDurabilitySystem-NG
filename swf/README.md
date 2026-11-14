# DurabilityMenu SWF

This project builds the Skyrim/Scaleform menu with the open-source MTASC
ActionScript 2 compiler. Adobe Animate is not required for normal code builds.

## Compiler

MTASC 1.15 is vendored at `tools/mtasc/mtasc.exe`, together with its standard
library and upstream documentation. To test another compatible build, set
`MTASC_PATH` to that executable; the environment override takes precedence.

## Build

Run:

```bat
build.cmd
```

The result is written to `bin\DurabilityMenu.swf`.

`build.cmd` uses `ExecutionPolicy Bypass` only for the child PowerShell process,
so it also works on machines where local PowerShell scripts are disabled.

`assets\DurabilityMenu.template.swf` is the asset container. It preserves the
exported `TextIns` movie-clip symbol required by `DurabilityWidget.as`. The build
copies this template and has MTASC replace its compiled ActionScript, so the
committed template must never be used as the distributable output directly.

The original `DurabilityMenu.fla` is retained as an archival source for the
library symbol. It is only needed if that visual asset or its linkage changes.
JPEXS Free Flash Decompiler can make those occasional SWF asset edits without
Animate.

## HUD data contract

The plugin and SWF must be shipped together. `Setup(x, y, scale, layout, reverse)`
sets layout preferences. `UpdateMenu(entries)` replaces the visible list with an
array of objects containing `slot`, `icon`, `text`, `value`, `color`, `data`,
`showName`, and `showHealth`. Slot identifies the equipment source, not a screen
position. A value of -1 suppresses health text. `data` is the optional inventory
icon-integration object.

The plugin filters hidden or empty equipment before sending the list. The SWF
creates holders on demand, reuses them, and hides surplus holders when the list
shrinks. An empty array clears the display. Positions follow array order in both
layouts; names and health visibility belong to each entry.

### In-game verification

- Hide spells and unarmed, equip a two-handed weapon, and check that no blank
  hand position remains between the weapon and armor.
- Switch between two-handed, dual-wield, spell, and empty hands; verify entries
  reappear and surplus icons, names, and health disappear.
- Toggle each equipment slot, breaking-only, and indestructible filters.
- Use different armor-name and weapon-name settings with shout hidden/shown;
  verify compacted entries keep the correct name visibility.
- Check vertical/horizontal and reversed layouts at different scales/positions.
- Hide all entries, then restore them; check for stale icons or text.
