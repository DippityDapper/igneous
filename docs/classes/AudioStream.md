# AudioStream

**Header:** `include/igneous/resources/AudioStream.hpp`  
**Implementation:** `src/resources/AudioStream.cpp`  
**Namespace:** `Engine`

## Overview

Wrapper around SDL_mixer `MIX_Audio` with playback properties.

## Members

| Member | Description |
|--------|-------------|
| `stream` | `MIX_Audio*` handle |
| `properties` | SDL properties ID for playback config |

## API

| Method | Description |
|--------|-------------|
| `AudioStream(stream, properties)` | Construct |
| `Play(gain)` | Play with volume gain |
| `SetProperties(properties)` | Update playback properties |
