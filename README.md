# Synopsis
**True Human Tribulation: Remedy** is an Action Adventure game where the
player must explore an map of maze like architecture, and defeat the boss
at the end. All the while, they must try their best to avoid unnecessary 
battles, and carefully manage their resources so they don't end up too 
weak to cross the metaphorical "finish line".

The game is currently in it's early pre-alpha stages. So feedback, and 
constructive criticism is very much appreciated. I'm currently focused on
building the game's core mechanics and systems, while also refining what's
built. Possible the most experimental phase of development as everything
is subject to change.

# Default Controls

## Keyboard

### Field
* **Arrow Keys** - Movement
* **Z Key** - Interact
* **X Key** - Open Menu

### Combat 
* **Arrow Keys** - Movement
* **Z Key** - Attack
* **X Key** - Defensive (Use in combination with directional inputs!)
* **A Key** - Light Technique
* **S Key** - Heavy Technique


## Gamepad
The controls detailed here are based on the SNES button layout, so they
may be different depending on what controller you use.

### Field
* **D-Pad** - Movement
* **A Button** - Interact
* **B Button** - Open Menu

### Combat 
* **D-Pad** - Movement
* **A Button** - Attack
* **B Button** - Defensive (Use in combination with directional inputs!)
* **X Button** - Light Technique
* **Y Button** - Heavy Technique


# Credits and Contacts

## Tyler Dillard
Role: Programmer, Designer, Pixel Artist, Composer, Sound Designer, pretty
much everything.

**Contact Email** - lorenarcdev@gmail.com

**Discord** - proarch. (With the period)

**Itch.io** - https://lorenarc.itch.io

**Gamejolt** - https://gamejolt.com/@LorenArc

**Twitter** - https://twitter.com/Tyler37961642

**Bluesky** - https://bsky.app/profile/lorenarc707.bsky.social

**Dev Channel** - https://www.youtube.com/channel/UCzR-5I3KE2YAV-Ilndl8Y9w


# Dependencies
If you wish to compile the game's source code for whatever reason. Either 
to try out upcoming updates early, access the super secret devmode, or 
add your own modifications; Then there's a couple external dependencies 
that need to met in order for compilation to be successful.

* C++17 - Standard Library
* Plog v1.1.10 - Header Only Logging Library
* Raylib v5.0 - Game Framework. Problems may occur on newer versions.
* Nlohmann Json - Modern C++ Json Library.

# Known Linux Issues
This game can also be built and ran on Linux. As of now, the game has been
confirmed to work perfectly fine on Fedora with minimum issues or 
differences; But even then, there's still lingering problem that aren't 
exactly gamebreaking, but it might be good to know in advance.

* Sometimes when loading the game, there's a chance where nothing will be
drawn at all. Like it would just be a completely black screen. The bug 
only seems to occur during startup, and there has been no confirmed case
of it happening during gameplay. The only known fix for this is to just
keep closing and running the game until it works.
