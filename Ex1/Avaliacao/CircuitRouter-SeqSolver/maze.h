/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * This code is an adaptation of the Lee algorithm's implementation originally included in the STAMP Benchmark
 * by Stanford University.
 *
 * The original copyright notice is included below.
 *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) Stanford University, 2006.  All Rights Reserved.
 * Author: Chi Cao Minh
 *
 * =============================================================================
 *
 * Unless otherwise noted, the following license applies to STAMP files:
 *
 * Copyright (c) 2007, Stanford University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of Stanford University nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY STANFORD UNIVERSITY ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL STANFORD UNIVERSITY BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * =============================================================================
 *
 * maze.h
 *
 * =============================================================================
 */


#ifndef MAZE_H
#define MAZE_H 1


#include "coordinate.h"
#include "grid.h"
#include "lib/list.h"
#include "lib/pair.h"
#include "lib/queue.h"
#include "lib/types.h"
#include "lib/vector.h"

typedef struct maze {
    grid_t* gridPtr;
    queue_t* workQueuePtr;   /* contains source/destination pairs to route */
    vector_t* wallVectorPtr; /* obstacles */
    vector_t* srcVectorPtr;  /* sources */
    vector_t* dstVectorPtr;  /* destinations */
} maze_t;


/* =============================================================================
 * maze_alloc
 * =============================================================================
 */
maze_t* maze_alloc ();


/* =============================================================================
 * maze_free
 * =============================================================================
 */
void maze_free (maze_t* mazePtr);


/* =============================================================================
 * maze_read (receives file to input the maze and another one to print)
 * -- Return number of path to route
 * =============================================================================
 */
long maze_read (maze_t* mazePtr, FILE* file_input, FILE* file_output);


/* =============================================================================
 * maze_checkPaths (receives file to output printing)
 * =============================================================================
 */
bool_t maze_checkPaths (maze_t* mazePtr, list_t* pathListPtr, FILE* file_output);


#endif /* MAZE_H */


/* =============================================================================
 *
 * End of maze.h
 *
 * =============================================================================
 */
