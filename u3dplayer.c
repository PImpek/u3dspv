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
#include <stdio.h>
#include "u3dspv.h"


int main(int argc, const char* argv[])
{
	int i,n,j;
	double* dbuf;
	char buf[3];
	FILE* f;
	double delay;
	f=fopen("out.u3d","rb");

	
	fread(buf,sizeof(char),3,f);
	fread( &i,sizeof( int),1,f);
	fread( &n,sizeof( int),1,f);
	
	U3D_INIT_(&n,&i);	
	
	printf("i=%d n=%d\n",i,n);
	dbuf = (double*) malloc((7*n+14)*sizeof(double));
	for ( j=0 ; j<i ; j++ )
	{
		fread(dbuf,sizeof(double),7*n+14,f);
		U3D_ADD_DATA_(&dbuf[0*n],&dbuf[1*n],&dbuf[2*n],&dbuf[3*n],&dbuf[4*n],&dbuf[5*n],&dbuf[6*n]);
	}
	
	delay = 0.1;
	U3D_PLAY_LOOP_(&delay);

	system("pause");
}
