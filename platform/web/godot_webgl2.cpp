/**************************************************************************/
/*  godot_webgl2.cpp                                                      */
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

#include "godot_webgl2.h"

#ifdef THREADS_ENABLED
#include "web_queue.h"
#endif

void godot_webgl2_glFramebufferTextureMultiviewOVRDirect(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint baseViewIndex, GLsizei numViews) {
#ifdef THREADS_ENABLED
	if (WebQueue::proxy_canvas_sync(godot_webgl2_glFramebufferTextureMultiviewOVR, target, attachment, texture, level, baseViewIndex, numViews)) {
		return;
	}
#endif
	godot_webgl2_glFramebufferTextureMultiviewOVR(target, attachment, texture, level, baseViewIndex, numViews);
}

void godot_webgl2_glFramebufferTextureMultisampleMultiviewOVRDirect(GLenum target, GLenum attachment, GLuint texture, GLint level, GLsizei samples, GLint baseViewIndex, GLsizei numViews) {
#ifdef THREADS_ENABLED
	if (WebQueue::proxy_canvas_sync(godot_webgl2_glFramebufferTextureMultisampleMultiviewOVR, target, attachment, texture, level, samples, baseViewIndex, numViews)) {
		return;
	}
#endif
	godot_webgl2_glFramebufferTextureMultisampleMultiviewOVR(target, attachment, texture, level, samples, baseViewIndex, numViews);
}

void godot_webgl2_glGetBufferSubDataDirect(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data) {
#ifdef THREADS_ENABLED
	if (WebQueue::proxy_canvas_sync(godot_webgl2_wrapper_glGetBufferSubData, target, offset, size, data)) {
		return;
	}
#endif
	godot_webgl2_wrapper_glGetBufferSubData(target, offset, size, data);
}
