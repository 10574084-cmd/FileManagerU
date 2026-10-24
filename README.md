# WiiU File Manager

Native Wii U homebrew file manager (C++ / WUT). This is an early implementation scaffold, not a verified WUP release.

## Build target
- Builds a native RPX through devkitPro WUT and GitHub Actions.
- SDL2-based UI scaffold.
- Includes helper functions for copying, moving, renaming, and deleting regular files.
- SD/USB paths are experimental and must be verified on real hardware.

## WUP packaging
A native .rpx is not a WUP Installer package. A valid installable WUP needs correct title metadata/content layout and a legitimate packaging/signing workflow. This repository does not fabricate tickets or fake WUPs. The workflow uploads the RPX artifact first.

## Limitations
The initial UI does not yet render filenames or provide complete copy/move/rename/delete dialogs. Directory copy and directory deletion are not implemented. The candidate paths /vol/external01 (SD) and /vol/storage_usb01 (USB) are not proof that those devices are mounted on every setup. USB is experimental until tested on-console.
