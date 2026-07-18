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
