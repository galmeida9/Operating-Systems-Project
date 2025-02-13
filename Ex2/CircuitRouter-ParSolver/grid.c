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
 * grid.c
 *
 * =============================================================================
 */

#define _GNU_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include "coordinate.h"
#include "grid.h"
#include "lib/types.h"
#include "lib/vector.h"
#include <pthread.h>


const unsigned long CACHE_LINE_SIZE = 32UL;
int id = 0;


/* =============================================================================
 * grid_alloc
 * =============================================================================
 */
grid_t* grid_alloc (long width, long height, long depth){
    grid_t* gridPtr;
    int i;

    gridPtr = (grid_t*)malloc(sizeof(grid_t));
    if (gridPtr) {
        gridPtr->width  = width;
        gridPtr->height = height;
        gridPtr->depth  = depth;
        long n = width * height * depth;
        long* points_unaligned = (long*)malloc(n * sizeof(long) + CACHE_LINE_SIZE);
        assert(points_unaligned);
        gridPtr->mutex_vector = (pthread_mutex_t *)malloc(n* sizeof(pthread_mutex_t));
        for (i=0; i<n; i++) pthread_mutex_init(&(gridPtr->mutex_vector[i]), NULL);
        gridPtr->points_unaligned = points_unaligned;
        gridPtr->points = (long*)((char*)(((unsigned long)points_unaligned
                                          & ~(CACHE_LINE_SIZE-1)))
                                  + CACHE_LINE_SIZE);
        memset(gridPtr->points, GRID_POINT_EMPTY, (n * sizeof(long)));
    }

    return gridPtr;
}

/* =============================================================================
 * grid_free
 * =============================================================================
 */
void grid_free (grid_t* gridPtr){
    int i;
    long n;
    n = (gridPtr->width) * (gridPtr->height) * (gridPtr->depth);

     
    for (i=0; i<n; i++) pthread_mutex_destroy(&(gridPtr->mutex_vector[i]));
    free(gridPtr->mutex_vector);
    free(gridPtr->points_unaligned);
    free(gridPtr);
}


/* =============================================================================
 * grid_copy
 * =============================================================================
 */
void grid_copy (grid_t* dstGridPtr, grid_t* srcGridPtr){
    assert(srcGridPtr->width  == dstGridPtr->width);
    assert(srcGridPtr->height == dstGridPtr->height);
    assert(srcGridPtr->depth  == dstGridPtr->depth);

    long n = srcGridPtr->width * srcGridPtr->height * srcGridPtr->depth;
    memcpy(dstGridPtr->points, srcGridPtr->points, (n * sizeof(long)));
}


/* =============================================================================
 * grid_isPointValid
 * =============================================================================
 */
bool_t grid_isPointValid (grid_t* gridPtr, long x, long y, long z){
    if (x < 0 || x >= gridPtr->width  ||
        y < 0 || y >= gridPtr->height ||
        z < 0 || z >= gridPtr->depth)
    {
        return FALSE;
    }

    return TRUE;
}


/* =============================================================================
 * grid_getPointRef
 * =============================================================================
 */
long* grid_getPointRef (grid_t* gridPtr, long x, long y, long z){
    return &(gridPtr->points[(z * gridPtr->height + y) * gridPtr->width + x]);
}

/* =============================================================================
 * grid_lockMutexPoint
 * =============================================================================
 */
int grid_lockMutexPoint (grid_t* gridPtr, long x, long y, long z){
    return pthread_mutex_lock(&(gridPtr->mutex_vector[(z * gridPtr->height + y) * gridPtr->width + x]));
}

/* =============================================================================
 * grid_unlockMutexPoint
 * =============================================================================
 */
int grid_unlockMutexPoint (grid_t* gridPtr, long x, long y, long z){
    return pthread_mutex_unlock(&(gridPtr->mutex_vector[(z * gridPtr->height + y) * gridPtr->width + x]));
}


/* =============================================================================
 * grid_getPointIndices
 * =============================================================================
 */
void grid_getPointIndices (grid_t* gridPtr, long* gridPointPtr, long* xPtr, long* yPtr, long* zPtr){
    long height = gridPtr->height;
    long width  = gridPtr->width;
    long area = height * width;
    long index3d = (gridPointPtr - gridPtr->points);
    (*zPtr) = index3d / area;
    long index2d = index3d % area;
    (*yPtr) = index2d / width;
    (*xPtr) = index2d % width;
}


/* =============================================================================
 * grid_getPoint
 * =============================================================================
 */
long grid_getPoint (grid_t* gridPtr, long x, long y, long z){
    return *grid_getPointRef(gridPtr, x, y, z);
}


/* =============================================================================
 * grid_isPointEmpty
 * =============================================================================
 */
bool_t grid_isPointEmpty (grid_t* gridPtr, long x, long y, long z){
    long value = grid_getPoint(gridPtr, x, y, z);
    return ((value == GRID_POINT_EMPTY) ? TRUE : FALSE);
}


/* =============================================================================
 * grid_isPointFull
 * =============================================================================
 */
bool_t grid_isPointFull (grid_t* gridPtr, long x, long y, long z){
    long value = grid_getPoint(gridPtr, x, y, z);
    return ((value == GRID_POINT_FULL) ? TRUE : FALSE);
}


/* =============================================================================
 * grid_setPoint
 * =============================================================================
 */
void grid_setPoint (grid_t* gridPtr, long x, long y, long z, long value){
    (*grid_getPointRef(gridPtr, x, y, z)) = value;
}


/* =============================================================================
 * grid_addPath
 * =============================================================================
 */
void grid_addPath (grid_t* gridPtr, vector_t* pointVectorPtr){
    long i;
    long n = vector_getSize(pointVectorPtr);

    for (i = 0; i < n; i++) {
        coordinate_t* coordinatePtr = (coordinate_t*)vector_at(pointVectorPtr, i);
        long x = coordinatePtr->x;
        long y = coordinatePtr->y;
        long z = coordinatePtr->z;
        grid_setPoint(gridPtr, x, y, z, GRID_POINT_FULL);
    }
}

/* =============================================================================
 * compare
 * =============================================================================
 */
int compare(const void* p1,const void* p2, void* grid){
    long** point1 = (long**) p1;
    long** point2 = (long**) p2;
    long x1,y1,z1, x2,y2,z2;
    grid_t* gridPtr = (grid_t*) grid;

    grid_getPointIndices(gridPtr, *point1, &x1, &y1, &z1);
    grid_getPointIndices(gridPtr, *point2, &x2, &y2, &z2);

    if (x1<x2) return -1;
    else if (x1>x2) return 1;
    else if (x1==x2){
            if (y1<y2) return -1;
            else if (y1>y2) return 1;
            else if(y1==y2){
                if (z1<z2) return -1;
                else if (z1>z2) return 1;
                else if (z1==z2) return 0;
            }
    }
    return 0;
    
}

/* =============================================================================
 * grid_addPath_Ptr
 * =============================================================================
 */
int grid_addPath_Ptr (grid_t* gridPtr, vector_t* pointVectorPtr){
    long i, j;
    long n = vector_getSize(pointVectorPtr), x,y,z;
    vector_t* vector_aux = vector_alloc(n);
    
    if (vector_copy(vector_aux, pointVectorPtr)==FALSE || vector_aux==NULL) return 0;
    
    vector_sort_r(vector_aux, 1, n-2, compare, (void*) gridPtr);

    for (i = 1; i < (n-1); i++){ 
        long* gridPointPtr = (long*)vector_at(vector_aux, i);
        if (gridPointPtr == NULL) return 0;
        grid_getPointIndices(gridPtr, gridPointPtr, &x, &y, &z);
        if (grid_lockMutexPoint(gridPtr, x, y, z)){
            fprintf(stderr, "Erro a obter mutex dum ponto na grid\n");
            pthread_exit(NULL);
        }
        if ((*gridPointPtr) == GRID_POINT_FULL){ 
            for (j = 1; j <= i; j++){
                long* gridPointPtr1 = (long*)vector_at(vector_aux, j);
                grid_getPointIndices(gridPtr, gridPointPtr1, &x, &y, &z);
                while (grid_unlockMutexPoint(gridPtr, x, y, z)) fprintf(stderr, "Erro a dar unlock a um mutex dum ponto da grid\n");
            }
            vector_free(vector_aux);
            return 0;
        }
    }
    

    for (i = 1; i < (n-1); i++){
        long* gridPointPtr = (long*)vector_at(vector_aux, i);
        if (gridPointPtr == NULL) return 0;
        *gridPointPtr = GRID_POINT_FULL; 
        grid_getPointIndices(gridPtr, gridPointPtr, &x, &y, &z);
        while (grid_unlockMutexPoint(gridPtr, x, y, z)) fprintf(stderr, "Erro a dar unlock a um mutex dum ponto da grid\n");
    }
    
    vector_free(vector_aux);
    return 1;
}


/* =============================================================================
 * grid_print
 * =============================================================================
 */
void grid_print (grid_t* gridPtr, FILE *fp){
    long width  = gridPtr->width;
    long height = gridPtr->height;
    long depth  = gridPtr->depth;
    long z;
    for (z = 0; z < depth; z++) {
        fprintf(fp, "[z = %li]\n", z);
        long x;
        for (x = 0; x < width; x++) {
            long y;
            for (y = 0; y < height; y++) {
                fprintf(fp,"%4li", *grid_getPointRef(gridPtr, x, y, z));
            }
            fputs("\n",fp);
        }
        fputs("\n",fp);
    }
}




/* =============================================================================
 *
 * End of grid.c
 *
 * =============================================================================
 */
