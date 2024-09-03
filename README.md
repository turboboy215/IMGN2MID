# IMGN2MID
## Absolute/Imagineering (GB/GBC) to MIDI converter

This tool converts music from Game Boy and Game Boy Color games using Absolute/Imagineering's sound engine to MIDI format.

It works with ROM images. To use it, you must specify the name of the ROM followed by the number of the bank containing the sound data (in hex).

Examples:
* IMGN2MID "Home Alone (U) [!].gb" 3
* IMGN2MID "Boy and His Blob, A - The Rescue of Princess Blobette (E) [!].gb" 3
* IMGN2MID "Simpsons, The - Escape from Camp Deadly (U) [!].gb" 7

This tool was created using the source code of the NES version of Home Alone 2 as a reference. The NES version of the sound engine works much like the Game Boy version.
As usual, a TXT-based "prototype" program, IMGN2TXT, is also included.

Supported games:
* 10-Pin Bowling (only the unreleased mono version has music, but is likely unused)
* The Adventures of Elmo in Grouchland
* The Adventures of Rocky and Bullwinkle and Friends
* Barbie: Game Girl
* Bart Simpson's Escape from Camp Deadly
* A Boy and His Blob: The Rescue of Princess Blobette
* Card Sharks (prototype, unused)
* Casper (GB)
* Deer Hunter
* Donald Duck in Maui Mallard
* F-18 Thunder Strike
* Frogger
* Home Alone
* Home Alone 2: Lost in New York
* InfoGenius Systems: Berlitz French Language Translator
* InfoGenius Systems: Berlitz German Language Translator
* InfoGenius Systems: Berlitz Japanese Language Translator
* InfoGenius Systems: Berlitz Spanish Language Translator
* InfoGenius Systems: Personal Organizer
* Jordan vs. Bird: One-on-One
* Mousetrap Hotel
* Phantom Air Mission
* The Ren & Stimpy Show: Space Cadet Adventures
* Sesame Street: Elmo's 123s
* Sesame Street: Elmo's ABCs
* Sesame Street Sports
* The Simpsons: Bart vs. the Juggernauts
* Star Trek: Generations: Beyond the Nexus
* Star Trek: The Next Generation
* Super Battletank
* Super Breakout
* Super Scrabble

## To do:
  * Panning support
  * Support for other versions of the sound engine (NES, Game Gear)
  * GBS file support
