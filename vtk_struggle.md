# VTK Struggle

> `fatal error: 'vtkExternalOpenGLRenderWindow.h' file not found`

1. Edit CMakeCache.txt
2. Find `Module_vtkRenderingExternal:BOOL=OFF`
3. Change to `Module_vtkRenderingExternal:BOOL=ON`
4. run `cmake.` and all that jazz

## Alternative

> Untested, but should be the same

Run cmake with `-DModule_vtkRenderingExternal=ON`