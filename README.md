# ydotool
Generic Linux command-line automation tool (no X!)

Forked to try to clean up the code and use a single GNU Makefile instead of CMake.

Might convert to C code to avoid need for extra C++ libraries ([libevdevPlus] and [uInputPlus]).
Then I could use `uinput` directly from the linux kernel.

#### Contents
- [Usage](#usage)
- [Examples](#examples)
- [Notes](#notes)
- [Build](#build)

## Usage
In most times, replace `x` with `y`. :P

Currently implemented command(s):
- `type` - Type a string
- `key` - Press keys
- `mousemove` - Move mouse pointer to absolute position
- `click` - Click on mouse buttons
- `recorder` - Record/replay input events

## Examples
Type some words:

    ydotool type 'Hey guys. This is Austin.'

Switch to tty1:

    ydotool key ctrl+alt+f1

Close a window in graphical environment:

    ydotool key Alt+F4

Move mouse pointer to 100,100:

    ydotool mousemove 100 100

Relatively move mouse pointer to -100,100:

    ydotool mousemove_relative -- -100 100

Mouse right click:

    ydotool click 2


## Notes
#### Runtime
This program requires access to `/dev/uinput`. This usually requires root permissions.

You can use it on anything as long as it accepts keyboard/mouse/whatever input. For example, wayland, text console, etc.

#### About the --delay option
ydotool works differently from xdotool. xdotool sends X events directly to X server, while ydotool uses the uinput framework of Linux kernel to emulate an input device.

When ydotool runs and creates an virtual input device, it will take some time for your graphical environment (X11/Wayland) to recognize and enable the virtual input device. (Usually done by udev)

So, if the delay was too short, the virtual input device may not got recognized & enabled by your graphical environment in time.

In order to solve this problem, I made a persistent background service, ydotoold, to hold a persistent virtual device, and accept input from ydotool. When ydotoold is unavailable, ydotool will work without it.

## Build
### Dependencies
- [uInputPlus]
- [libevdevPlus]
- boost::program\_options

### Compile

```bash
make -j $(nproc)
```

<!-- Links -->
[uinputplus]: https://github.com/YukiWorkshop/libuInputPlus
[libevdevplus]: https://github.com/YukiWorkshop/libevdevPlus
