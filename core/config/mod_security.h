/**************************************************************************/
/*  mod_security.h                                                        */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "core/string/ustring.h"
#include "core/templates/vector.h"

class ModSecurity {
private:
	static inline thread_local bool mod_context_active = false;
	static inline bool restrictions_enabled = false;

public:
	// Enable/disable mod restrictions globally
	static void set_restrictions_enabled(bool p_enabled) {
		restrictions_enabled = p_enabled;
	}

	static bool are_restrictions_enabled() {
		return restrictions_enabled;
	}

	// Set whether current thread is executing mod code
	static void set_mod_context(bool p_active) {
		mod_context_active = p_active;
	}

	static bool is_in_mod_context() {
		return mod_context_active && restrictions_enabled;
	}

	// Check if a path is allowed to be accessed from mod context
	static bool is_path_allowed(const String &p_path) {
		if (!is_in_mod_context()) {
			// Not in mod context, allow all access
			return true;
		}

		// In mod context with restrictions enabled
		String path_lower = p_path.to_lower();

		// Block access to res:// (game resources)
		if (path_lower.begins_with("res://") || path_lower.begins_with("uid://")) {
			return false;
		}

		// Only allow user://mods/ access
		if (path_lower.begins_with("user://mods/") || path_lower == "user://mods") {
			return true;
		}

		// Block all other user:// paths
		if (path_lower.begins_with("user://")) {
			return false;
		}

		// Block filesystem access
		return false;
	}

	// Get a user-friendly error message for blocked access
	static String get_access_denied_message(const String &p_path) {
		return vformat("Mod security: Access denied to '%s'. Mods can only access 'user://mods/' directory.", p_path);
	}
};
