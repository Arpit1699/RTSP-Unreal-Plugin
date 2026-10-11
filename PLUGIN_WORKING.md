# RTSPToVideo Plugin Working Guide

## Overview

RTSPToVideo is an Unreal Engine runtime plugin that decodes RTSP video with FFmpeg and displays the frames in UMG `UImage` widgets. The plugin exposes Blueprint APIs for starting and stopping streams, managing multiple stream slots, and applying zoom and pan controls.

## Requirements

- An Unreal Engine installation compatible with the project that consumes this plugin.
- A working RTSP endpoint reachable from the machine running the Unreal application.
- The FFmpeg development and runtime files expected by `Source/RTSPToVideo/RTSPToVideo.Build.cs` under `ThirdParty/FFmpeg`:
  - `include/`
  - `lib/Win64/` containing `avcodec.lib`, `avformat.lib`, `avutil.lib`, and `swscale.lib`
  - `bin/Win64/` containing the FFmpeg DLLs listed in the Build.cs file.

The current Build.cs configuration is Windows x64 specific. Other platforms need their own dependency paths, libraries, and runtime packaging rules before they can build this plugin.

## Install into an Unreal project

1. Copy the `RTSPToVideo` plugin folder into the project's `Plugins` directory. Create that directory if it does not exist.
2. Confirm the `ThirdParty/FFmpeg` files listed above are present. The build rules fail if required files are missing.
3. Open the `.uproject` and enable **RTSPToVideo** in **Edit → Plugins** if it is not already enabled.
4. Restart the editor when prompted and allow Unreal to compile the plugin.

## Use from Blueprint

1. Create a Widget Blueprint based on `RTSPStreamWidget`.
2. Add one or more `Image` widgets and mark them **Is Variable**. Assign each image to a stream slot's `VideoTarget` and enter its `StreamURL` (for example, `rtsp://<host>:<port>/<path>`).
3. Add the zoom/pan material to the widget's `BaseRTSPMaterial` if those controls are needed. The material must expose a texture parameter named `VideoTexture`, scalar parameter `Zoom`, and vector parameter `Pan`. `Content/Materials/M_RTSPZoomPan.uasset` is provided as a starting point.
4. Call `StartAllStreams` when the widget is ready. Call `StopAllStreams` when playback should end. Individual slots can be controlled with `StartStream` and `StopStream`.
5. Bind to `OnSlotFirstFrame` to know when a stream has produced a visible frame. The event provides the slot index and stream ID.

`AddAndStartStream(URL, VideoTarget)` adds and starts a slot at runtime and returns its slot index. `SetZoom(SlotIndex, Value)` clamps the zoom to the widget's `MinZoom` and `MaxZoom`; `SetPan(SlotIndex, Vector2D)` updates the pan offset.

The stream manager can also be accessed through the Game Instance subsystem: call `GetRTSPSubsystem` with a world context object, then call `GetStreamManager`.

## Repository layout

- `RTSPToVideo.uplugin` — plugin descriptor and runtime module declaration.
- `Source/RTSPToVideo/` — module, Blueprint widget, stream manager, and Game Instance subsystem.
- `ThirdParty/FFmpeg/` — FFmpeg headers, import libraries, and runtime binaries.
- `Content/` — included zoom/pan material assets.
- `Userguide/` — supplementary setup document.

## Development notes

- The module currently declares Windows x64 FFmpeg dependencies in `RTSPToVideo.Build.cs`.
- Do not commit Unreal-generated `Binaries/` or `Intermediate/` build output as source. Keep the plugin descriptor, source, required content assets, and third-party files needed to reproduce a build.
- Check the FFmpeg distribution's license and redistribution terms before publishing its binaries with the plugin.
- Verify the plugin in a clean Unreal project and test both stream start and stop behavior before tagging a release.

## Troubleshooting

- **Build reports a missing FFmpeg path or file:** Check the `ThirdParty/FFmpeg` folder layout and the required Win64 library and DLL names in `RTSPToVideo.Build.cs`.
- **The widget stays blank:** Confirm the RTSP URL is reachable, the `VideoTarget` is assigned, and the widget calls `StartAllStreams` (or `StartStream`). Use `OnSlotFirstFrame` to distinguish connection/decode delays from widget setup issues.
- **Zoom and pan have no visible effect:** Assign the zoom/pan material and verify its parameter names match `VideoTexture`, `Zoom`, and `Pan`.

## Release checklist

- [ ] Build and load the plugin in a clean project using the intended Unreal Engine version.
- [ ] Verify RTSP playback, first-frame notification, and stream shutdown.
- [ ] Verify packaging includes all required FFmpeg runtime DLLs.
- [ ] Review third-party license notices and redistribution rights.
- [ ] Ensure generated build folders and local-only files are excluded from the release commit.
