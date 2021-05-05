# Reset OPL2/3

This utility resets an OPL2/3 chip (including stopping all playing notes).

Without an argument, it assumes 0x220 as the base address:

```
C:\> resetopl
No argument given, assuming base at 0x220
Resetting OPL3 at 0x220
```

You can override the base by providing it as an argument:
```
C:\> resetopl 0x240
Resetting OPL3 at 0x240
```

The utility detects if the OPL-chip is present and will abort if the chip
is not detected:
```
C:\> resetopl 0x230
OPL not detected at 0x230
```

This tool is compiled with Turbo C 2.01.

Download a compiled version from "Releases".
