# C 3D-Projection

**Warning**: This project is still a WIP and not totally finished

This project is a simple 3D "engine" written in C for the [Blackbox](https://blackbox.hackclub.com) YSWS, which is centered around the theme constraints. It's a device with a resolution of only 8x8 pixels. The math is similar to my [Python 3D projection](https://github.com/Illuminum2/pygame-3d-projection) projection. 

## Features

- **Projection**: Matrices for projection onto the display
- **Rotation**: Quaternions for rotation of the camera to avoiding gimbal lock.

## Difficulties

These are a few of the technical difficulties I faced while working on this project. The things mentioned here are not supported by the Blackbox editor:

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

1. Copy the ``main.c` file

2. Paste it into the [editor](https://blackbox.hackclub.com/editor/)

3. Hit the sound icon, and then press start

## Controls

- **Arrow Key Up**: Move forward/Look up
- **Arrow Key Down**: Move backward/Look down
- **Arrow Key Left**: Move left/Look left
- **Arrow Key Right**: Move right/Look right
- **X**: Switch between movement and rotation

## To-Do

- **Implement controls**: Done ~70%
- **Potentially add sound**

## License

This project is licensed under the [MIT License](LICENSE.md).