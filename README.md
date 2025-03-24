# C 3D-Projection

**Warning**: This project is still a WIP and not totally finished

This project is a simple 3D "engine" written in C for the [Blackbox](https://blackbox.hackclub.com) YSWS, which is centered around the theme constraints. It's a device with a resolution of only 8x8 pixels. The math is similar to my [Python 3D projection](https://github.com/Illuminum2/pygame-3d-projection) projection. 

## Features

- **Projection**: Matrices for projection onto the display
- **Rotation**: Quaternions for rotation of the camera to avoiding gimbal lock.

_**Notes**: Currently using direct rotation matrix manipulation as a temporary fix!_

## Difficulties

These are a few of the technical difficulties I faced while working on this project. These features were not supported by the Blackbox editor at the time I wrote my code:

- **No console for debugging**:
- **No external libraries**: External libraries are **NOT** allowed, this project was made without:
- - `math.h`: sin(), cos(), pow(), sqrt()
- - `stdlib.h`: malloc(), realloc(), calloc(), free()
- **No pointers**: There are issues with * and &, no pointer were used
- **No arrays**
- **No structs**
- **No function prototypes**: Function prototyping does not seem to work
- **No typecasts**: ToInt() function had to be implemented
- **No preprocessor directives**: Don't seem to work
- **Others**: Some other convenience features of the C language are also not usable

## How to Run

Option 1: You can access the project [here](https://blackbox.hackclub.com/editor/?code=2e5f68) on the Blackbox website.

Option 2: Copy the `main.c` file. Paste it into the [editor](https://blackbox.hackclub.com/editor/).

## Controls

- **Arrow Key Up**: 1: Move forward 2: Move Up
- **Arrow Key Down**: 1: Move backward 2: Move Down
- **Arrow Key Left**: 1: Move left 2:Look left
- **Arrow Key Right**: 1: Move right 2: Look right
- **X**: Switch between 1 and 2

## To-Do

- **Potentially add sound**

## License

This project is licensed under the [MIT License](LICENSE.md).
