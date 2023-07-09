# ABYSS
A terminal based roguelite game written in C and rendered using the ncurses library

<img src="https://i.imgur.com/pGVTeMY.png"  width="600" height="420"> 

## ABOUT
Abyss is a roguelite dungeon crawler that has been modernised with rpg elements and a flexible turn based combat system using ASCII art

## Installation
Abyss requires the ncurses library (https://invisible-island.net/ncurses/announce.html#h2-overview).
ncurses should already be installed with Linux distributions but can be installed using your package manager. Ncurses can be installed on OSX using homebrew with `brew install ncurses`. Ncurses is not currently supported by windows and therefore neither is abyss.

Abyss is written in C and as long as ncurses is installed, should compile on any platform that supports it. The repository includes a makefile, running `make build` will create the abyss executable that can be ran from the terminal using `./abyss`.

## CONTROLS
Abyss uses WASD for movement and ui navigation. Generally E is used for select and Q is used to back out of menus and also to quit the game.

## PROGRESSION
Abyss saves your progress everytime you move to a new floor, there is only one save slot so if a new character is created and progresses to a new floor, it will overwriite the current save.

## FEATURES
Character Creation
- Create your character, choose a name and optionally assign skill points
<img src="https://i.imgur.com/hMKUYT9.png" width="600" height="420">

- A fleshed out inventory and equipment system
<img src="https://i.imgur.com/KxnpOI8.png" width="600" height="420">

- Sprawling randomised dungeons full of monsters and enemies
<img src="https://i.imgur.com/JxQzmwo.png" width="600" height="420">

- Tense combat that includes dynamic elements such as status effects, chances to miss, and a variety of spells, weapons, and items
<img src="https://i.imgur.com/ZyWclGA.png" width="600" height="420">

## LEGEND

### Enemies
- `g` Goblin
- `s` Spider
- `S` Succubus
- `o` Orc
- `r` Rogue
- `l` Lamia
- `D` Dragon

### Others
- `v` Floor transition
- `i` Item
- `@` Player

## FUTURE PLANS
This is considered feature complete, and will not recieve additional major updates, any future updates will likely be smaller bug fixes and art improvements.

## LICENSE
This work is protected by the Creative Commons Attribution-NonCommercial 4.0 International Public License (CC BY-NC 4.0). This means you can share or modify this work as long as attribution is given and it is not used for any commercial use.
