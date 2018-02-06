## 3D Navigable Scene
### Blaire Bosely, Bri Brown-Richardson, Tristan Lopus

This program renders a simple, 3D scene through which an observer may navigate with a first-person view. The scene consists of two houses, each with a different car in front of it. They are bounded by a four-sided fence.

## Collidable Objects
### The following objects detect collisions and do not allow the observer to pass through them
- House 1
- Peugeot car (in front of House 1)
- BMW car (in front of House 2)
- 4 sides of fence

## Navigation
The user controls the view with the mouse and movement with the W and S keys. Note, the view is rotated by moving the mouse only; not by clicking and dragging. When any mouse button is held down, rotation is disabled. This allows the user to reposition the mouse in relation to the view, in case the mouse gets to the edge of the screen but further rotation is desired.

## Controls
- Idle mouse movement (no buttons pressed): rotate observer's View
- Right Arrow: rotate view right
- Left Arrow: rotate view left
- Up Arrow: increase y position
- Down Arrow: decrease y position
- W: move forward
- S: move backward
- G: toggle visibility of developer guides (axes and gridlines)
- R: reset observer's position to default location
- Q: Quit program

## Known Bugs
1. The observer is able to move backwards through collidable objects. When a collision is detected, only forward motion is prevented. Backward motion is allowed because the user needs to be able to separate from the collidable object in order to re-enable forward motion. A more elegant solution would be to determined if the surface the observer has collided with is in front of them or behind them and disable either forward or backward motion, respectively.
2. The user is able to move down under the ground. While unrealistic, this is purposely left in, as it offers a kind of cool view of the scene
3. When the program is first launched, nothing is visible until the user rotates the scene into view.
