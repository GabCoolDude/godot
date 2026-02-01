/**************************************************************************/
/*  web_queue.h                                                           */
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

#ifdef THREADS_ENABLED

#include <emscripten/proxying.h>
#include <emscripten/threading.h>
#include <pthread.h>
#include <cstdint>
#include <cstring>
#include <new>
#include <type_traits>

#include "core/os/memory.h"
#include "core/os/thread.h"
#include "core/templates/simple_type.h"

class WebQueue {
private:
	static inline pthread_t default_thread;
	static inline pthread_t web_main_thread;
	static inline pthread_t web_canvas_thread;
	static inline em_proxying_queue *queue = nullptr;
	static inline Thread::ID canvas_thread_id;

	friend class OS_Web;
	friend class DisplayServerWeb;

	static void initialize() {
		// Register the thread that initialized OS_Web as default.
		default_thread = pthread_self();
		web_main_thread = default_thread;
		web_canvas_thread = default_thread;

		queue = em_proxying_queue_create();
	}

	static void setup_canvas_thread(bool p_canvas_on_runtime) {
		// Register the thread that creates the display server as the main thread.
		web_main_thread = pthread_self();
		// Forward calls to the thread with WebGL context.
		if (p_canvas_on_runtime) {
			// Canvas is on the browser thread.
			web_canvas_thread = emscripten_main_runtime_thread_id();
			// Query runner thread for its thread id.
			proxy_generic_async(queue, web_canvas_thread, assign_canvas_thread_id);
			execute_queue();
		} else {
			// Offscren canvas is enabled and the canvas is inside the current thread.
			web_canvas_thread = web_main_thread;
			// Use the current thread id.
			assign_canvas_thread_id();
		}
	}

	static void reset_threads() {
		// After the display server is destroyed forward all calls to the thread
		// that initialzed OS_Web.
		web_main_thread = default_thread;
		web_canvas_thread = default_thread;
		assign_canvas_thread_id();
	}

	static void destroy() {
		if (queue) {
			em_proxying_queue_destroy(queue);
			queue = nullptr;
		}
	}

	static void assign_canvas_thread_id() {
		canvas_thread_id = Thread::get_caller_id();
	}

	static _FORCE_INLINE_ bool is_canvas_thread() {
		return canvas_thread_id == Thread::get_caller_id();
	}

public:
	// Should lower level variants with emscripten_proxy_get_system_queue() exist?
	// Or to a runner emscripten_main_runtime_thread_id() itself if "proxy_to_pthread" is enabled?

	// Forward calls to the main thread.
	// Returns true if the call was forwarded.
	template <typename R, typename... P, typename... VarArgs>
	static bool proxy_main_sync(R (*p_func)(P...), VarArgs &&...p_args) {
		if (!Thread::is_main_thread()) {
			proxy_generic_sync(queue, web_main_thread, p_func, std::forward<VarArgs>(p_args)...);
			return true;
		}
		return false;
	}
	// Forward calls to the main thread.
	// Returns true if the call was forwarded.
	template <typename R, typename... P, typename... VarArgs>
	static bool proxy_main_async(R (*p_func)(P...), VarArgs &&...p_args) {
		if (!Thread::is_main_thread()) {
			proxy_generic_async(queue, web_main_thread, p_func, std::forward<VarArgs>(p_args)...);
			return true;
		}
		return false;
	}

	// Forward calls to the thread with canvas.
	// Returns true if the call was forwarded.
	template <typename R, typename... P, typename... VarArgs>
	static bool proxy_canvas_sync(R (*p_func)(P...), VarArgs &&...p_args) {
		if (!is_canvas_thread()) {
			proxy_generic_sync(queue, web_canvas_thread, p_func, std::forward<VarArgs>(p_args)...);
			return true;
		}
		return false;
	}
	// Forward calls to the thread with canvas.
	// Returns true if the call was forwarded.
	template <typename R, typename... P, typename... VarArgs>
	static bool proxy_canvas_async(R (*p_func)(P...), VarArgs &&...p_args) {
		if (!is_canvas_thread()) {
			proxy_generic_async(queue, web_canvas_thread, p_func, std::forward<VarArgs>(p_args)...);
			return true;
		}
		return false;
	}

	static void execute_queue() {
		emscripten_proxy_execute_queue(queue);
	}

private:
	// Generic implementations
	template <typename R, typename... P, typename... VarArgs>
	static _FORCE_INLINE_ void proxy_generic_sync(em_proxying_queue *p_queue, pthread_t p_thread, R (*p_func)(P...), VarArgs &&...p_args) {
		using FuncType = R (*)(P... args);

		// Declare pointers as const pointers to accept const references.
		void const *argptrs[1 + sizeof...(p_args)];

		// Store only pointers, as emscripten_proxy_sync waits for the function call to finish.
		argptrs[0] = &p_func;
		size_t index = 1;
		([&] {
			argptrs[index] = &p_args;
			index += 1;
		}(),
				...);

		emscripten_proxy_sync(
				p_queue,
				p_thread,
				[](void *p_userdata) {
					// Remove const from all stored pointers and add it when unpacking if needed.
					void **argptrs = static_cast<void **>(p_userdata);

					FuncType func = *static_cast<FuncType *>(argptrs[0]);
					size_t index = 1;
					// Dereference pointers after the lambda call, it allows references to bind directly
					// to the actual values.
					func(*[&] {
						// Cast it back to a pointer, while preserving original constness.
						using ArgMaybeConst = std::remove_reference_t<VarArgs>;
						ArgMaybeConst *arg = static_cast<ArgMaybeConst *>(argptrs[index]);
						index += 1;
						return arg;
					}()...);
				},
				argptrs);
	}

	template <typename R, typename... P, typename... VarArgs>
	static _FORCE_INLINE_ void proxy_generic_async(em_proxying_queue *p_queue, pthread_t p_thread, R (*p_func)(P...), VarArgs &&...p_args) {
		using FuncType = R (*)(P... args);
		constexpr size_t func_size = sizeof(FuncType);
		constexpr size_t buffer_size = func_size + (0 + ... + sizeof(GetSimpleTypeT<VarArgs>));

		// Allocate buffer for function pointer + all arguments.
		uint8_t *buffer = static_cast<uint8_t *>(memalloc(buffer_size));

		// Copy function pointer to the buffer.
		memcpy(buffer, &p_func, func_size);

		// Begin offset after stored function pointer.
		size_t offset = func_size;
		([&] {
			using ArgT = GetSimpleTypeT<VarArgs>;
			// Copy arguments to the buffer while invoking appropriate copy or move constructors if they exist.
			memnew_placement((buffer + offset), ArgT(std::forward<VarArgs>(p_args)));
			offset += sizeof(ArgT);
		}(),
				...);

		emscripten_proxy_async(
				p_queue,
				p_thread,
				[](void *p_userdata) {
					uint8_t *buffer = static_cast<uint8_t *>(p_userdata);
					FuncType func;
					// Copy function pointer back.
					memcpy(&func, buffer, func_size);

					size_t offset_call = func_size;
					// Dereference pointers after the lambda call, references will be bound to the copy inside the buffer.
					func(*[&] {
						using ArgMaybeConst = std::remove_reference_t<VarArgs>;
						using ArgT = GetSimpleTypeT<VarArgs>;
						// std::launder is needed to prevent compiler optimizations and
						// indicate that the value with type ArgT * actually exists here.
						ArgT *arg = std::launder(reinterpret_cast<ArgT *>(buffer + offset_call));
						offset_call += sizeof(ArgT);
						// Add const to the argument if it was passed with const, preserves correct semantics
						// and won't allow you to pass const argument to non-const function parameter.
						return static_cast<ArgMaybeConst *>(arg);
					}()...);

					size_t offset_dstr = func_size;
					([&] {
						using ArgT = GetSimpleTypeT<VarArgs>;
						// Call destructor on created with memnew_placement values.
						memdestroy_at(std::launder(reinterpret_cast<ArgT *>(buffer + offset_dstr)));
						offset_dstr += sizeof(ArgT);
					}(),
							...);

					// Deallocate buffer.
					memfree(buffer);
				},
				buffer);
	}
};

#endif