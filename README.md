# Flower Matching
A two-player turn based flower matching game that I made for my wife on our 15th anniversary.

![Screenshot](/screenshot.png?raw=true "Screenshot")

The goal is to create a row of four or more matching flowers horizontally or vertically by swapping adjacent flowers (the swapping can be done horizontally, vertically or diagonally) in as few moves as possible. To start with, players are given 1 move. If both players pass, then they are allowed two moves. If they both pass again, they are allowed three moves, etc, until a player sees a way they can make a match within the given number of moves and does so.

Player 1 uses the mouse, Player 2 uses the keyboard. It is Player 1's turn first (indicated by the "Player 1: 1 Moves Left" on the bottom-right of the screen). If you attempt to swap tiles but it does not result in a match and you have run out of moves, the tiles will not be swapped and it will be the other player's turn.

## Controls

Mouse:
* The first click selects the current tile (indicated by the border turning white)
* The second click swaps tiles the current tile with the selected tile

Keyboard:
* The arrow keys move the current tile
* The first click selects the current tile (indicated by the border turning white)
* The second click swaps tiles the current tile with the selected tile

Passing:
* For either player to "pass", press the "P" key on the keyboard when it is their turn.

## Attributions

The vector artwork (black-and-white) was created by `nitut78` on The Noun Project and can be found here: https://thenounproject.com/nitu786/collection/abstract-flower-set/. I did the coloring in the Sketch app.

## To-Do

I rushed this out pretty quickly, so there are a lot of things that can be done to improve it:

* Allow diagonal matches?
* Add Sounds
* Add Animation
* Add Controller Support
* Hint to the player what moves are legal
* Put the two players scores left-aligned and right-aligned, indicate turn via highlighted background
* Refactor DRY violations out of the code
