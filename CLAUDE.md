# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

See the [MansOS wiki](https://github.com/edi-riga/MansOS/wiki) for additional documentation.

## What is MansOS

MansOS is an operating system for small embedded systems, wireless sensor networks (WSN), and IoT devices. It targets resource-constrained platforms including MSP430-based systems and Atmega/Arduino hardware, with support for radio transceivers (CC2420, CC1101, etc.) and IPv6 networking via uIPv6. Features include optional preemptive multitasking, low-energy sleep modes, runtime reprogramming, and an interactive shell.

## Setup

Install the toolchain for a specific platform:

```bash
cd apps/demo/Blink
make setup telosb    # installs the telosb toolchain (may request sudo)
make setup pc        # installs the PC build toolchain
```

## Building

Each application has its own `Makefile` that includes the central build system at `mos/make/Makefile`. Build targets are platform names:

```bash
make pc              # build for PC (host machine simulation)
make telosb          # build for TelosB hardware
make pc run          # build and run on PC (Ctrl+C to stop)
make telosb upload   # build and upload to connected hardware
```

Meta-commands (run from any app directory):

```bash
make targets         # list all available platforms
make options         # display configuration options
make help            # general help
```

Build output goes to `build/` inside the app directory. Use `make <platform>-clean` to clean.

**Supported platforms:** `pc`, `telosb`, `sadmote`, `sm3`, `atmega`, `z1`, `launchpad`, `xm1000`, and others defined in `mos/make/Makefile.platforms`.

## Application Structure

Every app needs exactly three files:

**`Makefile`** — defines `SOURCES`, `APPMOD`, `PROJDIR`, `MOSROOT`, then includes the main build system:
```makefile
SOURCES = main.c
APPMOD = Blink
PROJDIR = $(CURDIR)
ifndef MOSROOT
  MOSROOT = $(PROJDIR)/../../..
endif
include ${MOSROOT}/mos/make/Makefile
```

**`config`** — compile-time feature flags (can be empty; inherits from `mos/make/config.default`):
```
USE_LEDS=y
```

**`main.c`** — entry point is `appMain()`, not `main()`. Include `stdmansos.h` as the standard header:
```c
#include "stdmansos.h"

void appMain(void) {
    for (;;) {
        ledToggle();
        mdelay(1000);
    }
}
```

The watchdog timer is disabled by default via `watchdogStop()`. `apps/Empty/` is a template for new projects.

## Running Tests

Tests live in `apps/tests/`. Run them the same way as any app:

```bash
cd apps/tests/AlarmTest
make pc run
```

For hardware targets, use `make telosb upload`. There is no automated test runner; tests are individual applications that must be built and run per-platform.

## Architecture

The layered architecture (from low to high):

- **`mos/arch/`** — CPU-specific code: `msp430/`, `avr/`, `pc/`. Contains startup, interrupts, and low-level peripheral drivers per architecture.
- **`mos/chips/`** — Device drivers for specific ICs (radio transceivers, sensors, memory, displays). Each chip gets its own subdirectory.
- **`mos/hil/`** — Hardware Interface Layer: unified API for GPIO (`digital.h`), LEDs, UART, SPI, I2C, ADC, timers, etc. This is what application code uses instead of arch/chip code directly.
- **`mos/kernel/`** — Core OS: boot, alarms (`alarms.h`, `timing.h`), sleep modes, threads (`threads/`), and protothreads (`protothreads/`).
- **`mos/net/`** — Networking stack: MAC protocols, routing, socket API, time synchronization, uIPv6.
- **`mos/lib/`** — Utility libraries: buffers, queues, assertions, debug printing, messaging frameworks, NMEA.
- **`mos/fs/`** and **`mos/fatfs/`** — Filesystem support.
- **`mos/platforms/`** — Per-platform board configuration (maps platform name to arch + chip combo).

**Applications** in `apps/`:
- `demo/` — simple examples
- `tests/` — subsystem tests
- `advanced/` — complex examples
- `seal/` — apps using the SEAL domain-specific language (compiled by `tools/seal/`)

## Key Conventions

- **Hardware access goes through HIL** (`mos/hil/`) rather than arch or chip code directly. When adding hardware support, implement the HIL interface in `arch/<arch>/` and wire it up in the relevant `platforms/Makefile.<platform>`.
- **GPIO** uses macros from `digital.h`: `pinAsOutput(port, pin)`, `pinSet()`, `pinRead()`, etc. Use `PIN_DEFINE(name, port, pin)` to create named-pin convenience functions (`nameAsOutput()`, `nameRead()`, `nameToggle()`, etc.).
- **Alarms** API (`mos/include/alarms.h`): `alarmInit(alarm, cb, param)`, `alarmSchedule(alarm, ms)`, `alarmRemove(alarm)`. Time functions: `getTimeMs()`, `getTimeMs64()`, `getTimeSec()`, `getSyncTimeMs()`.
- **`config` feature flags** in each app directory override defaults from `mos/make/config.default`. User-global overrides go in `~/.mansos.siteopts`.
- **Threading model:** both POSIX-style threads (`USE_THREADS=1`) and protothreads are supported; threads are not available on the `pc` platform.
- **No CI/CD.** Verification is done by building for `pc` and running locally, plus manual hardware testing.
