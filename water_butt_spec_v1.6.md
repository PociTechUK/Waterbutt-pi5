# Waterbutt Controller – Spec v1.6 (Scenery & Interactivity Addendum)

This addendum builds on v1.5 and defines visual polish and lightweight interactivity that won’t blow the Pi 5 budget. It’s optional, modular, and can be rolled out incrementally.

## Goals
- Keep 60 FPS on Pi 5 in kiosk mode.
- Zero-crash UX: if data or network is missing, show a graceful animated state instead of raw errors.
- Everything can be toggled by flags (for profiling and troubleshooting).

## Scene & Lighting
- **Day/Night Tint**: Scene color grading driven by local time or HA `sun.sun`:
  - Day → neutral; Evening → warm; Night → cool/low gamma.
- **Weather Hooks** (optional): If HA weather entity exists:
  - Cloudy → dimmer lighting, calmer ripples.
  - Rain → surface splashes + optional “drizzle” overlay.
  - Sunny → subtle specular/glare boost on water.

## Water
- **Surface Ripples**: Lightweight normal-map or simple Gerstner waves (2–3 combined).
- **Impact Impulses**: Touch and toy impacts add a decaying ripple.
- **Perf Guardrail**: Limit active impulses and LOD-drop normal frequency if FPS < target.

## Toys
- **Draggable Toys**: Pick up/throw a duck/ball/brick. Cap active toys to N to keep perf.
- **Seasonal Skins** (optional): Pumpkin/Xmas bauble via texture swap, toggled by date.

## Touch
- **Single-tap Ripple**: Any tap on water adds a ripple pulse at coordinates.
- **Drag-to-Throw**: Press-drag-release vector applies impulse to nearest toy.
- **(Future) Gestures**: Reserve pinch/rotate for later; keep disabled by default.

## Fallback / Error UX
- **HA Offline**: Show playful “rain fill” animation with subtle banner “Home Assistant offline” and retry spinner.
- **Level Missing**: Freeze water as “iced” surface with small info toast “Level unavailable”.
- **Auto-Restart**: If UI process exits, systemd restarts it (no desktop fallback).

## Config Flags (env or INI)
```ini
[visuals]
day_night = true
weather_hooks = false
ripples = true
ripple_count_max = 64
toy_limit = 8
seasonal_skins = false

[ui]
touch_tap_ripple = true
drag_throw = true

[failsafe]
show_offline_anim = true
show_level_missing_ice = true
autorestart = true