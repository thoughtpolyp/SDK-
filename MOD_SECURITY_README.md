# Mod Security System

This Godot fork includes a security system to restrict mod access to game resources.

## Overview

The mod security system prevents mods from accessing the main game resources (`res://`) while allowing them to read and write to a designated mods directory (`user://mods/`).

## Usage

### Enabling Mod Restrictions

Before loading any mod code, enable the mod security restrictions:

```cpp
#include "core/config/mod_security.h"

// Enable mod restrictions globally
ModSecurity::set_restrictions_enabled(true);
```

### Running Mod Code

When executing mod code (scripts, plugins, etc.), set the mod context:

```cpp
// Before executing mod code
ModSecurity::set_mod_context(true);

// ... execute mod code here ...

// After executing mod code
ModSecurity::set_mod_context(false);
```

### Access Rules

When mod restrictions are enabled and code is running in mod context:

- ✅ **ALLOWED**: `user://mods/` - Mods can read and write to this directory
- ❌ **BLOCKED**: `res://` - Main game resources are completely inaccessible
- ❌ **BLOCKED**: `user://` (except `user://mods/`) - Other user data is protected
- ❌ **BLOCKED**: Filesystem paths - Direct filesystem access is blocked

### Example Implementation

```cpp
// In your mod loading system
void load_mod(const String &mod_path) {
    // Enable restrictions if not already enabled
    if (!ModSecurity::are_restrictions_enabled()) {
        ModSecurity::set_restrictions_enabled(true);
    }

    // Set mod context before loading
    ModSecurity::set_mod_context(true);

    // Load and execute mod script
    Ref<GDScript> mod_script = ResourceLoader::load(mod_path);
    if (mod_script.is_valid()) {
        // Execute mod...
    }

    // Restore context after execution
    ModSecurity::set_mod_context(false);
}
```

### Disabling Restrictions

For your own game code (not mod code), ensure restrictions are not active:

```cpp
// Disable mod context when running game code
ModSecurity::set_mod_context(false);

// Or disable restrictions entirely during development
ModSecurity::set_restrictions_enabled(false);
```

## Implementation Details

The mod security system works by:

1. Hooking into `FileAccess::open()`, `FileAccess::exists()`, `DirAccess::open()`, and `DirAccess::exists()`
2. Checking if mod context is active using thread-local storage
3. Validating requested paths against allowed patterns
4. Blocking access and showing error messages for unauthorized paths

## Thread Safety

The mod context is tracked per-thread using thread-local storage, allowing different threads to have different security contexts simultaneously.

## Security Considerations

- Always enable restrictions before loading untrusted mod code
- Set mod context appropriately for each execution
- The user://mods/ directory should be the designated location for all mod files
- Mods should store all their data within user://mods/ subdirectories
