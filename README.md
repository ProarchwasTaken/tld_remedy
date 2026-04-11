# Synopsis
**True Human Tribulation III: Remedy** is an Action Adventure game that 
has a slight focus on resource mangagement. The player must conserve their 
limited resources the best they can, so they don't end up too weak to 
cross the finish line. Either from playing skillfully in combat, or 
avoiding it whenever possible, it's entirely up to you.

The game is currently in it's Alpha stages. Which means the core mechanics
had been established and finalized. Nevertheless, feedback is still very 
much appreciated, and since this is an Open-Source project, helpful 
contributions are welcome as well.


# Default Controls
These are the default controls when you first open the game. In the 
settings menu, (Which can be opened from the Title Screen and Camp 
Menu) they can be remapped to your personal liking.

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
* **D Key** - Light Assist
* **C Key** - Heavy Assist

* **Tab Key** - Pause
* **Left Shift Key** - Open Item Menu 

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
* **L Button** - Light Assist
* **R Button** - Heavy Assist

* **Start Button** - Pause
* **Select Button** - Open Item Menu


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


# MacOS Warning
I have no intentions to maintain an MacOS version of the game. Mostly 
don't really have access to the proper MacOS envioroment to test of game 
on, nor do I have motivation to set something like that up on my non-mac
laptop.

The game can be compiled for MacOS thanks to a recent contributor though,
but don't be surprised if there are any issues.
