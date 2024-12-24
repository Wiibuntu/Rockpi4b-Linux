/*
 * Copyright (c) 2013
 * Phillip Lougher <phillip@squashfs.org.uk>
 *
 * This work is licensed under the terms of the GNU GPL, version 2. See
 * the COPYING file in the top-level directory.
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/pagemap.h>
#include <linux/buffer_head.h>
#include "page_actor.h"

struct squashfs_page_actor *squashfs_page_actor_init(struct page **page,
	int pages, int length, void (*release_pages)(struct page **, int, int))
{
	struct squashfs_page_actor *actor = kmalloc(sizeof(*actor), GFP_KERNEL);

	if (actor == NULL)
		return NULL;

	actor->length = length ? : pages * PAGE_SIZE;
	actor->buffer = buffer;
	actor->pages = pages;
	actor->next_page = 0;
	actor->squashfs_first_page = cache_first_page;
	actor->squashfs_next_page = cache_next_page;
	actor->squashfs_finish_page = cache_finish_page;
	return actor;
}

/* Implementation of page_actor for decompressing directly into page cache. */
static void *direct_first_page(struct squashfs_page_actor *actor)
{
	actor->next_page = 1;
	return actor->pageaddr = kmap_atomic(actor->page[0]);
}

static void *direct_next_page(struct squashfs_page_actor *actor)
{
	if (actor->pageaddr)
		kunmap_atomic(actor->pageaddr);

	return actor->pageaddr = actor->next_page == actor->pages ? NULL :
		kmap_atomic(actor->page[actor->next_page++]);
}

static void direct_finish_page(struct squashfs_page_actor *actor)
{
	if (actor->pageaddr)
		kunmap_atomic(actor->pageaddr);
}

struct squashfs_page_actor *squashfs_page_actor_init_special(struct page **page,
	int pages, int length)
{
	struct squashfs_page_actor *actor = kmalloc(sizeof(*actor), GFP_KERNEL);

	if (actor == NULL)
		return NULL;

	actor->length = length ? : pages * PAGE_SIZE;
	actor->page = page;
	actor->pages = pages;
	actor->next_page = 0;
	actor->pageaddr = NULL;
	actor->squashfs_first_page = direct_first_page;
	actor->squashfs_next_page = direct_next_page;
	actor->squashfs_finish_page = direct_finish_page;
	return actor;
}
