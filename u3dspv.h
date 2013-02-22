/* Copyright (c) 2013, Bartosz Foder, (bartosz@foder.pl)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef U3DSPV_H
#define U3DSPV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#if defined (EXPORT_DLL) && (WIN32)
 #define DLL_EXPORT __declspec(dllexport)
#elif defined (WIN32)
 #define DLL_EXPORT __declspec(dllimport)
#else
 #define DLL_EXPORT
#endif

/*
 *
 * name: u3d_init_
 * initialization of the visualisation window.
 * @param
 * ssx : Space Size in X direction.
 * ssy : Space Size in Y direction.
 * ssz : Space Size in Z direction.
 * non : Number Of Nodes
 * itr : number of ITRations.
 * @return
 * 0 : when thread with window started normally.
 * other : failure during creating thread.
 *
 */
int DLL_EXPORT U3D_INIT_(int* non, int* itr);
/*
 *
 * name: u3d_set_title_
 * sets window caption (up to 255 characters);
 * @param
 * str : caption
 * size: fortran pass this argument implicitly
 */
void DLL_EXPORT U3D_SET_TITLE_(const char* str, unsigned int size);
/*
 *
 * name: u3d_add_data_
 * @param
 * xX : vectors of position (non)
 * vX : vectors of velocity (non)
 * prop : vector of property (non), i.e. temperature
 *
 */
void DLL_EXPORT U3D_ADD_DATA_(double* x1, double* x2, double* x3, double* v1, double* v2, double* v3, double* prop);
/*
 *
 * name: u3d_join
 * ends visualization
 *
 */
void DLL_EXPORT U3D_JOIN_();
/*
 *
 * name: u3d_play_loop_
 * plays buffered data in loop. Useful after ;
 * @param
 * delay - delay beetween frames changes, in [ms]
 *
 */
void DLL_EXPORT U3D_PLAY_LOOP_(double* delay);
/*
 *
 * name: u3d_store_data_
 * stores data in file
 * @param
 * fname - path to file
 * size: fortran pass this argument implicitly
 *
 */
void DLL_EXPORT U3D_STORE_DATA_(const char* fname, unsigned int size);
#endif //U3DSPV_H
