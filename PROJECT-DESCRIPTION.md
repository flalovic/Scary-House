# Scary House

mr22002 - Filip Lalović

A small interactive 3D scene inspired by a haunted interior. The project uses a custom OpenGL-based engine to render several imported 3D models, move through the scene with a first-person camera, and trigger a timed wizard appearance event through keyboard input.

## Controls

W / A / S / D -> move the camera through the scene  
Mouse -> rotate the camera view  
Mouse wheel -> zoom in and out  
F2 -> toggle the GUI and cursor  
K -> trigger the wizard appearance sequence  
Esc -> exit the application

## Features

### Fundamental:

- [x] Model with lighting  
- [x] Spotlight-based lighting with adjustable color and on/off control through the GUI  
- [x] K --- 2 seconds --- wizard appears --- 2 seconds --- wizard disappears

### Group A:

- [ ] Frame-buffers with post-processing  
- [ ] Off-screen Anti-Aliasing  
- [ ] Parallax Mapping  
- [x] Bloom with the use of HDR

### Group B:

- [ ] Deferred Shading  
- [ ] Point Shadows  
- [ ] SSAO

### Engine improvement:

- [x] The engine can load and compile its own shaders from the resources/shaders directory.

## Models:

[Scary interior scene](https://sketchfab.com/3d-models/scary-interior-fa7d60e783764ecfb15039bba6c05306)     
[Pillar](https://sketchfab.com/3d-models/muqarnas-pillar-81f9be447fa14c66b7b384ca32953fc0)  
[Magma ball](https://sketchfab.com/3d-models/projectile-magma-ball-915c10ddb81543eeaa122f7e159eccc5)  
[Wizard](https://sketchfab.com/3d-models/dark-necromancer-corrupted-staff-skulls-10c3bef4113b4e128ad41c9fdcab74e3)

## Textures

Textures are loaded from the image data bundled with the imported GLTF model assets.