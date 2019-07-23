# Flower Matching
A two-player turn based flower matching game that I made for my wife on our 15th anniversary.

![Screenshot](/screenshot.png?raw=true "Screenshot")

The goal is to create a row of **four or more matching flowers** horizontally or vertically by swapping adjacent flowers (the swapping can be done horizontally, vertically or diagonally) in as few moves as possible.

Player 1 uses the mouse, Player 2 uses the keyboard. Both players can play simultaneously. Each gets a total of 25 moves.

## Points
When a match is made, the player gets 1 point for if it is 4-in-a-row, 2 points for 5-in-a-row, 3 points for 6-in-a-row, etc.

## Controls

Mouse:
* The first click selects the current tile (indicated by the border turning white)
* The second click swaps tiles the current tile with the selected tile

Keyboard:
* The arrow keys move the current tile
* Press the spacebar to select the current tile (indicated by the border turning white)
* Press the spacebar again to swap the current tile with the selected tile

## Download

You can download the game [for Windows here](https://github.com/prust/flower-matching/releases/download/v0.1.1/flower-matching-win.zip), or [for OS X here](https://github.com/prust/flower-matching/releases/download/v0.1.1/flower-matching-osx.zip).

## Attributions

The vector artwork (black-and-white) was created by `nitut78` on The Noun Project and can be found here: https://thenounproject.com/nitu786/collection/abstract-flower-set/. I did the coloring in the Sketch app.

## To-Do

I rushed this out pretty quickly, so there are a lot of things that can be done to improve it:

* Allow diagonal matches?
* Add Sounds
* Add Animation
* Add Controller Support
* Try sliding entire rows up/down/left/right instead of swapping tiles
* Put the two players scores left-aligned and right-aligned
* Refactor DRY violations out of the code
