/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "101_team2",
    /* First member's full name */
    "1",
    /* First member's email address */
    "1",
    /* Second member's full name (leave blank if none) */
    "Classbinu",
    /* Second member's email address (leave blank if none) */
    "classbinu@gmail.com"};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Basic constants and macros */
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 12)

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char *)(bp) - (3 * WSIZE))
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - (2 * DSIZE))

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - (3 * WSIZE))))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE(((char *)(bp) - (2 * DSIZE))))

#define PREDP(bp) ((char *)(bp) - (DSIZE))
#define SUCCP(bp) ((char *)(bp) - (WSIZE))

static void *heap_listp;
static void *free_listp = NULL;

static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t allocated_size);
static void *first_fit(size_t allocated_size);
static void place(void *bp, size_t allocated_size);

static void append_free_block(void *bp);
static void remove_free_block(void *bp);

int mm_init(void)
{
    if ((heap_listp = mem_sbrk(6 * WSIZE)) == (void *)-1)
        return -1;

    PUT(heap_listp, 0);
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE * 2, 1));
    PUT(heap_listp + (2 * WSIZE), NULL);
    PUT(heap_listp + (3 * WSIZE), NULL);
    PUT(heap_listp + (4 * WSIZE), PACK(DSIZE * 2, 1));
    PUT(heap_listp + (5 * WSIZE), PACK(0, 1));
    free_listp = heap_listp + DSIZE;

    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;
    return 0;
}

static void *extend_heap(size_t words)
{
    void *bp;
    size_t size;

    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((bp = mem_sbrk(size)) == (void *)-1)
        return NULL;

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    return coalesce(bp);
}

void *mm_malloc(size_t size)
{
    size_t allocated_size;
    size_t extendsize;
    char *bp;

    if (size == 0)
        return NULL;

    if (size <= DSIZE)
        allocated_size = 3 * DSIZE;
    else
        allocated_size = DSIZE * ((size + (2 * DSIZE) + (DSIZE - 1)) / DSIZE);

    if ((bp = find_fit(allocated_size)) != NULL)
    {
        place(bp, allocated_size);
        return bp;
    }

    extendsize = MAX(allocated_size, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, allocated_size);
    return bp;
}

void append_free_block(void *bp)
{
    PUT(SUCCP(bp), free_listp);
    PUT(PREDP(bp), NULL);
    PUT(PREDP(free_listp), bp);
    free_listp = bp;
}

void remove_free_block(void *bp)
{
    if (bp == free_listp)
    {
        PUT(PREDP(SUCCP(bp)), NULL);
        free_listp = SUCCP(bp);
    }
    else
    {
        PUT(SUCCP(GET(PREDP(bp))), GET(SUCCP(bp)));
        PUT(PREDP(GET(SUCCP(bp))), GET(PREDP(bp)));
    }
}

static void *find_fit(size_t allocated_size)
{
    return first_fit(allocated_size);
}

static void *first_fit(size_t allocated_size)
{
    void *bp;

    for (bp = free_listp; GET_ALLOC(HDRP(bp)) != 1; bp = GET(SUCCP(bp)))
    {
        if (allocated_size <= GET_SIZE(HDRP(bp)))
        {
            return bp;
        }
    }
    return NULL;
}

static void place(void *bp, size_t allocated_size)
{
    size_t current_size = GET_SIZE(HDRP(bp));

    remove_free_block(bp);
    if ((current_size - allocated_size) >= (3 * DSIZE))
    {
        PUT(HDRP(bp), PACK(allocated_size, 1));
        PUT(FTRP(bp), PACK(allocated_size, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(current_size - allocated_size, 0));
        PUT(FTRP(bp), PACK(current_size - allocated_size, 0));
        append_free_block(bp);
    }
    else
    {
        PUT(HDRP(bp), PACK(current_size, 1));
        PUT(FTRP(bp), PACK(current_size, 1));
    }
}

void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc)
    {
        return bp;
    }

    else if (prev_alloc && !next_alloc)
    {
        
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }

    else if (!prev_alloc && next_alloc)
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    else
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}

void *mm_realloc(void *bp, size_t size)
{
    void *old_bp = bp;
    void *new_bp;
    size_t copySize;

    new_bp = mm_malloc(size);
    if (new_bp == NULL)
        return NULL;
    copySize = GET_SIZE(HDRP(old_bp));
    if (size < copySize)
        copySize = size;
    memcpy(new_bp, old_bp, copySize);
    mm_free(old_bp);
    return new_bp;
}