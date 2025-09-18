# Raspberry Pi Water Butt — Unified Project Spec (v1.5)

> Targets **Raspberry Pi 5 (8 GB)** and **10" 1280×800** touchscreen.  
> Headless-friendly; dev from Mac; NVMe boot; PoE later.  
> **New in v1.5**: RAM‑backed precomputation & caching strategy + gameplay scaling (higher toy cap with replace‑oldest spawn policy).

---

## 1) Vision & Scope
Ultra‑realistic water/weather/lighting/foliage around a 3D garden water butt, with an orbit camera. Toys remain intentionally simple/fun. Target **60 fps @ 1280×800**.

---

## 2) What’s New vs v1.4
- **RAM Caches** to trade spare 8 GB memory for speed & stability (remove frame spikes).  
- **Max toys** increased (safe to 8–10) with **replace‑oldest** spawn policy to keep perf flat.  
- Menu options added for toy limits & policy.

---

## 3) RAM‑Backed Precomputation & Caching
Use spare RAM (~1–1.5 GB budget) to precompute and cache data; per‑frame work becomes lookups/blends.

### 3.1 Water
- **Impulse kernels**: pre‑baked ripple stamps for rain/toy entries.  
- **FFT ripple tiles**: a pool of normal map tiles blended for variety.  
- **Buoyancy weights**: precomputed sample weights for common collider shapes (sphere/box/capsule).

### 3.2 Lighting & Atmosphere
- **Sun/moon ephemeris table**: a day’s azimuth/elevation precomputed → index by time (no trig in frame loop).  
- **Sky LUTs**: sunrise/noon/sunset/night gradients; fade between.  
- **Cloud shadow masks**: scrolling noise masks generated once at start.

### 3.3 Foliage
- **Wind phase tables**: per blade group sin/cos phases; shader just indexes.  
- **Tree gust poses**: pre-baked bone offsets for gust strength 0..1; interpolate.  
- **Snow accumulation**: layered masks (0/25/50/75/100%); blend instead of redrawing every frame.

### 3.4 Weather/Particles
- **Spawn buffers**: pre-rolled pseudo‑random positions/velocities for rain/snow; spawn = increment index.  
- **Lightning**: precomputed exposure curve used on flashes.

### 3.5 System
- **Replay buffer**: last 30 s sim state in RAM.  
- **Double‑buffered sim**: compute frame N+1 while GPU renders frame N.

**Target outcome**: GPU 10–12 ms typical (≤14–15 ms storm), CPU 2–3 ms. Rails rarely trigger; 60 fps held.

---

## 4) Gameplay Scaling — Toys
Keep water realism intact; scale toys by keeping physics simple.

### 4.1 Limits & Policy
- **max_active_toys**: default **5**, user‑configurable up to **10** (menu: Physics → Max active toys).  
- **toy_spawn_policy**: { **replace_oldest**, reject } (menu selectable).  
- **ring buffer** implementation: fixed array of toy slots with head/tail indices.

### 4.2 Lifecycle
- On spawn and at cap with **replace_oldest**:  
  1) Mark oldest toy for removal (set `fade_out=1`, disable interactions).  
  2) Cross‑fade splash audio; optional small ripple on despawn.  
  3) Remove after fade (e.g. 0.5–1.0 s) and reuse slot.
- **Physics cost** stays O(max_active_toys); no inter‑toy collisions beyond simple separation.

### 4.3 Menu Additions
- **Physics panel**: `max_active_toys` (1–10), `toy_spawn_policy` (replace_oldest/reject).  
- Per‑toy density/drag/spin as in v1.4.

---

## 5) Performance Rails (unchanged logic, updated notes)
Triggered if GPU > 16 ms for 3 frames:
1) Reflection res → 480×300.  
2) Water grid → 192².  
3) SSR far‑rays off; keep near‑field.  
4) Grass → 2k, tree leaf LOD low.  
5) Internal render scale → 0.9×.

Rails may temporarily override menu selections; UI shows a yellow badge.

---

## 6) Files & Persistence
- Config JSON: `config/runtime.json` (created from `config/defaults.json` if absent).  
- Profiles: `config/profiles/<name>.json`.  
- New fields:
```json
{
  "physics": {
    "max_active": 8,
    "spawn_policy": "replace_oldest"
  },
  "cache": {
    "enable": true,
    "memory_mb": 1024
  }
}
```

---

## 7) Open Items
- Validate memory footprint of caches (cap at 1.5 GB).  
- Confirm stable 60 fps with 10 toys under storm + SSR on (expect rails to step in).  
- Despawn VFX polish (subtle ripple & foam).

---
