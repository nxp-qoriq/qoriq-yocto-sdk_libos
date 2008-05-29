/**
 * Memory allocation using a simple alloc-and-never-free pointer.
 *
 * This is used for "remote heap" functionality such as valloc(),
 * even when dlmalloc is used.
 */
/* Copyright (C) 2007-2008 Freescale Semiconductor, Inc.
 * Author: Scott Wood <scottwood@freescale.com>
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <libos/bitops.h>

typedef struct {	
	uintptr_t start, end;
} allocator;

#if !defined(CONFIG_LIBOS_MALLOC)
static allocator heap;
#endif

static allocator virtual;
static uint32_t alloc_lock;

static void *__alloc(allocator *a, size_t size, size_t align)
{
	register_t saved = spin_lock_critsave(&alloc_lock);

	uintptr_t new_start = (a->start + align - 1) & ~(align - 1);
	void *ret = (void *)new_start;
	new_start += size;

	if (new_start > a->end || new_start <= a->start)
		ret = NULL;
	else
		a->start = new_start;

	spin_unlock_critsave(&alloc_lock, saved);
	return ret;
}

#if !defined(CONFIG_LIBOS_MALLOC)
void *simple_alloc(size_t size, size_t align)
{
	return __alloc(&heap, size, align);
}
#endif

void *valloc(unsigned long size, unsigned long align)
{
	return __alloc(&virtual, size, align);
}

#if !defined(CONFIG_LIBOS_MALLOC)
void simple_alloc_init(void *start, size_t size)
{
	heap.start = (uintptr_t)start;
	heap.end = heap.start + size - 1;
}
#endif

void valloc_init(unsigned long start, unsigned long end)
{
	virtual.start = (uintptr_t)start;
	virtual.end = end;
}