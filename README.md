# smoll-gltf
glTF parser to intake binary files exported from blender

Warning
-------
this has no security for bounds checking.
the intedind purpose of this file is for you to load glb files that you have full control over to turn them into a different format.
bounds checking to prevent buffer overflows could be added in the future.


requirements
------------
- stdint.h
- stdlib.h
- string.h
