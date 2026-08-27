# Scary House

mr22002 - Filip Lalović

An interactive 3D scene inspired by a haunted interior. The project uses a custom OpenGL-based engine to
render imported GLTF models, provide first-person camera movement, apply HDR lighting and post-processing,
cast omnidirectional point-light shadows, and trigger a timed wizard appearance event.

## Demonstration

[View the Scary House demonstration on Google Drive](https://drive.google.com/file/d/1-4d-uhnRe3TCJKG1FG71dBrJ97cZvoxD/view?usp=drive_link)

## Controls

| Input | Action |
| --- | --- |
| W / A / S / D | Move the camera through the scene |
| Mouse | Rotate the camera view |
| Mouse wheel | Zoom in and out |
| F2 | Toggle the GUI and cursor |
| K | Trigger the wizard appearance sequence |
| Esc | Exit the application |

## Features

### Fundamental

- [x] Model with lighting  
- [x] Spotlight and HDR point-light illumination with GUI controls for enabled state and colors
- [x] K --- 2 seconds --- wizard appears --- 2 seconds --- wizard disappears

### Group A

- [ ] Frame-buffers with post-processing  
- [ ] Off-screen Anti-Aliasing  
- [ ] Parallax Mapping  
- [x] Bloom using HDR rendering and adjustable exposure

### Group B

- [ ] Deferred Shading  
- [x] Omnidirectional point shadows using a depth cubemap and 20-sample PCF
- [ ] SSAO

### Engine improvements

- [x] Engine-owned shader resources are resolved through the `engine-` naming convention and a configurable
      engine resource path
- [x] Reusable HDR Bloom framebuffer and post-processing pipeline
- [x] Reusable point-shadow depth cubemap lifecycle and rendering pass
- [x] Safe framebuffer resizing when the application window is minimized

## Models

- [Scary interior scene](https://sketchfab.com/3d-models/scary-interior-fa7d60e783764ecfb15039bba6c05306)
- [Pillar](https://sketchfab.com/3d-models/muqarnas-pillar-81f9be447fa14c66b7b384ca32953fc0)
- [Magma ball](https://sketchfab.com/3d-models/projectile-magma-ball-915c10ddb81543eeaa122f7e159eccc5)
- [Wizard](https://sketchfab.com/3d-models/dark-necromancer-corrupted-staff-skulls-10c3bef4113b4e128ad41c9fdcab74e3)

## Textures

Textures are loaded from the image data bundled with the imported GLTF model assets. Model files are kept
locally under `app/resources/models/` and are not stored in Git because of their size.