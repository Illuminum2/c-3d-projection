# C 3D-Projection

**Warning**: This project is still a WIP and not totally finished

This project is a simple 3D "engine" written in C for the [Blackbox](https://blackbox.hackclub.com), which has a resolution of only 8x8 pixels. It's similar to my [Python 3D projection](https://github.com/Illuminum2/pygame-3d-projection). 

## Features

- **Projection**: Matrices for projection onto the display
- **Rotation**: Quaternions for rotation of the camera to avoiding gimbal lock.

## Difficulties

- **No external libraries**: The blackbox editor does **NOT** allow external libraries, which means this project was written without:
- - `math.h`: sin(), cos(), pow(), sqrt()
- - `stdlib.h`: malloc(), realloc(), calloc(), free()
- **No pointers**: There are issues with * and & in the Blackbox editor, so no pointer were used
- **No arrays**: Arrays currently do not work in the Blackbox editor, everything is array free
- **No structs**: Structs do not work at the moment in the Blackbox editor, everything is struct free
- **No preprocessor directives**: At the moment user made preprocessor directives do not work, I used constants instead
- **Others**: Some other convenience features of the C language are also not supported

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

- **Implement controls**
- **Potentially add sound**

## License

This project is licensed under the [MIT License](LICENSE.md).