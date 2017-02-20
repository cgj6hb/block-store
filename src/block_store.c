#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "block_store.h"
#include "bitmap.h"

// Define these here instead of hardcoding to keep program scalable
#define BLOCK_SIZE 256
#define BLOCK_COUNT 256

// Define block_store_t
typedef struct block_store {
    void* block;
} block_store_t;

/*
 *  This creates a new BS device, ready to go
 */
block_store_t* block_store_create() {
    // Allocate space for 256 block addresses
    block_store_t* bs = malloc(sizeof(block_store_t) * BLOCK_COUNT);

    // Allocate 256 bytes for each block
    // These are not contiguous, but are addressable via an array of blocks
    for (int i = 0; i < BLOCK_COUNT - 1; i++) {
        bs[i].block = malloc(BLOCK_SIZE);
    }

    // Allocate 255th block for bitmap and set bit 255 accordingly
    bs[BLOCK_COUNT - 1].block = bitmap_create(BLOCK_COUNT);
    bitmap_set(bs[BLOCK_COUNT - 1].block, BLOCK_COUNT - 1);
    return bs;
}

/*
 * Destroys the provided block storage devices
 */
void block_store_destroy(block_store_t* const bs) {
    // Check param
    if (!bs) {
        return;
    }

    // Destroy bitmap (this will deallocate)
    bitmap_destroy(bs[BLOCK_COUNT - 1].block);

    // Deallocate the first 255 blocks
    for (int i = 0; i < BLOCK_COUNT - 1; i++) {
        free(bs[i].block);
    }

    // Finally, deallocate all the block pointers
    free(bs);
    return;
}

/*
 * Searches for a free block, marks it as in use
 * and returns the block's id
 */
size_t block_store_allocate(block_store_t* const bs) {
    // Check param
    if (!bs) {
        return SIZE_MAX;
    }

    // Find index of first zero through the bitmap_ffz function
    size_t block = bitmap_ffz(bs[BLOCK_COUNT - 1].block);

    if (block == SIZE_MAX) { // This means there are no free blocks
        return SIZE_MAX;
    } else { // We've found a free block. Set bitmap and return index
        bitmap_set(bs[BLOCK_COUNT - 1].block, block);
        return block;
    }
}

/*
 * Attempts to allocate the requested block id
 */
bool block_store_request(block_store_t* const bs, const size_t block_id) {
    // Check params
    if (!bs || block_id > BLOCK_COUNT - 1) {
        return false;
    }

    // Test the requested block to see if it's available or not
    if (!bitmap_test(bs[BLOCK_COUNT - 1].block, block_id)) {
        bitmap_set(bs[BLOCK_COUNT - 1].block, block_id); // If free, set bitmap
        return true;
    } else {
        return false; // No go
    }
}

/*
 * Frees the specified block
 */
void block_store_release(block_store_t* const bs, const size_t block_id) {
    // Check param
    if (!bs) {
        return;
    }

    // Could've checked to see if it was already cleared, but the same
    // result is achieved regardless
    bitmap_reset(bs[BLOCK_COUNT - 1].block, block_id);
}

/*
 * Counts the number of blocks marked as in use
 */
size_t block_store_get_used_blocks(const block_store_t* const bs) {
    // Check param
    if (!bs) {
        return SIZE_MAX;
    }

    // Call the provided bitmap function to return number of set blocks
    size_t used_blocks = bitmap_total_set(bs[BLOCK_COUNT - 1].block);

    // Need to subtract 1 because we don't care that the bitmap's index is set
    return used_blocks - 1;
}

/*
 * Counts the number of blocks marked free for use
 */
size_t block_store_get_free_blocks(const block_store_t* const bs) {
    // Check param
    if (!bs) {
        return SIZE_MAX;
    }

    // This number is just the difference of 256 and used_blocks that we found
    size_t free_blocks = BLOCK_COUNT - bitmap_total_set(bs[BLOCK_COUNT - 1].block);
    return free_blocks;
}

/*
 * Returns the total number of user-addressable blocks
 * (since this is constant, you don't even need bs object)
 */
size_t block_store_get_total_blocks() {
    // This is constant. Easy!
    return BLOCK_COUNT - 1;
}

/*
 * Reads data from the specified block and writes it to designated buffer
 */
size_t block_store_read(const block_store_t* const bs, const size_t block_id, void* buffer) {
    // Check params
    if (!bs || block_id > BLOCK_COUNT - 1 || !buffer) {
        return 0;
    }

    // Simply copy the memory and return success
    memcpy(buffer, bs[block_id].block, BLOCK_SIZE);
    return BLOCK_SIZE;
}

/*
 * Reads data from buffer and writes it to designated block
 */
size_t block_store_write(block_store_t* const bs, const size_t block_id, const void* buffer) {
    // Check params
    if (!bs || block_id > BLOCK_COUNT - 1 || !buffer) {
        return 0;
    }

    // block_id is already tested in tests.cpp, so we can assume block is free
    // Simply copy the memory and return success
    memcpy(bs[block_id].block, buffer, BLOCK_SIZE);
    return BLOCK_SIZE;
}

block_store_t* block_store_deserialize(const char* const filename) {
    // Check param
    if (!filename) {
        return NULL;
    }

    return NULL;
}

size_t block_store_serialize(const block_store_t* const bs, const char* const filename) {
    // Check params
    if (!bs || !filename) {
        return 0;
    }

    return 0;
}
