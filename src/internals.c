#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "internals.h"

typedef struct _data_parameters TDataParameters;
struct _data_parameters{
	int pos;
	int size;
	char *str;
	char **eachOne;
	char *sep;

};


static void iteratorParameters(TParameters *pars){
	TDataParameters *data = pars->data;
	data->pos=-1;

}


static char *nextParameters(TParameters *pars){
	TDataParameters *data = pars->data;
	char *str = NULL;
	data->pos++;

	if (data->pos < data->size ){
		//str = (char *)malloc(strlen(data->eachOne[data->pos])+1);
		//strcpy(str,data->eachOne[data->pos]);
		str = data->eachOne[data->pos];
	}
	return str;

}

static void splitParameters(TParameters *pars){
	TDataParameters *data = pars->data;

	char *cpy = data->str;
	char *ant=cpy, *value;
	int len, i=0;

	len = strlen(data->str);

	if (data->str[len-1]!=data->sep[0]){
		printf("|----------------------------------------|\n");
		printf("|Error: Internal.c:Parameters incomplete...missing %c|\n",data->sep[0]);
		printf("|----------------------------------------|\n");
		exit(0);
	}

	while((cpy=strstr(cpy,data->sep))!=NULL){
		i++;
		cpy++;
	}

	data->size=i;
	data->eachOne = (char **)malloc(sizeof(char*)*data->size);

	i=0;
	cpy=ant;
	value = strtok_r(cpy,data->sep,&ant);
	while(value){
		data->eachOne[i]=(char *)malloc(strlen(value)+1);
		strcpy(data->eachOne[i],value);
		value = strtok_r(NULL,data->sep,&ant);
		i++;
	}
//	i=0;
//	cpy=ant;
//	while((cpy=strstr(cpy,pars->private_data->sep))!=NULL){
//		len = cpy-ant;
//
//		pars->private_data->eachOne[i]=(char *)malloc(len+2);
//		strncpy(pars->private_data->eachOne[i],ant,len);
//		pars->private_data->eachOne[i][len+1]='\0';
//		i++;
//
//		cpy++;
//		ant=cpy;
//	}
}


static void destroyParameters(TParameters *p){
	TDataParameters *data = p->data;

	int i;

	free(data->str);
	free(data->sep);

	for(i=0;i<data->size;i++)
		free(data->eachOne[i]);
	free(data->eachOne);

	free(p->data);
	free(p);
}

TParameters *createParameters(const char *str, const char *sep){

	TParameters *p = (TParameters *)malloc(sizeof(TParameters));

	TDataParameters *data = (TDataParameters *)malloc(sizeof(struct _data_parameters));

	p->data = data;

	int len= strlen(sep)+1;
	data->sep = (char *)malloc(len);

	len = strlen(str)+1;
	data->str = (char *)malloc(len);

	data->eachOne = NULL;
	data->size=0;

	strcpy(data->sep,sep);
	strcpy(data->str,str);


	p->dispose = destroyParameters;
	p->next = nextParameters;
	p->iterator = iteratorParameters;

	splitParameters(p);

	return p;
}


/*
  Copyright (C) 1999, 2000, 2002 Aladdin Enterprises.  All rights reserved.

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  L. Peter Deutsch
  ghost@aladdin.com

 */
/* $Id: md5.c,v 1.6 2002/04/13 19:20:28 lpd Exp $ */
/*
  Independent implementation of MD5 (RFC 1321).

  This code implements the MD5 Algorithm defined in RFC 1321, whose
  text is available at
	http://www.ietf.org/rfc/rfc1321.txt
  The code is derived from the text of the RFC, including the test suite
  (section A.5) but excluding the rest of Appendix A.  It does not include
  any code or documentation that is identified in the RFC as being
  copyrighted.

  The original and principal author of md5.c is L. Peter Deutsch
  <ghost@aladdin.com>.  Other authors are noted in the change history
  that follows (in reverse chronological order):

  2002-04-13 lpd Clarified derivation from RFC 1321; now handles byte order
	either statically or dynamically; added missing #include <string.h>
	in library.
  2002-03-11 lpd Corrected argument list for main(), and added int return
	type, in test program and T value program.
  2002-02-21 lpd Added missing #include <stdio.h> in test program.
  2000-07-03 lpd Patched to eliminate warnings about "constant is
	unsigned in ANSI C, signed in traditional"; made test program
	self-checking.
  1999-11-04 lpd Edited comments slightly for automatic TOC extraction.
  1999-10-18 lpd Fixed typo in header comment (ansi2knr rather than md5).
  1999-05-03 lpd Original version.
 */


#undef BYTE_ORDER	/* 1 = big-endian, -1 = little-endian, 0 = unknown */
#ifdef ARCH_IS_BIG_ENDIAN
#  define BYTE_ORDER (ARCH_IS_BIG_ENDIAN ? 1 : -1)
#else
#  define BYTE_ORDER 0
#endif

#define T_MASK ((md5_word_t)~0)
#define T1 /* 0xd76aa478 */ (T_MASK ^ 0x28955b87)
#define T2 /* 0xe8c7b756 */ (T_MASK ^ 0x173848a9)
#define T3    0x242070db
#define T4 /* 0xc1bdceee */ (T_MASK ^ 0x3e423111)
#define T5 /* 0xf57c0faf */ (T_MASK ^ 0x0a83f050)
#define T6    0x4787c62a
#define T7 /* 0xa8304613 */ (T_MASK ^ 0x57cfb9ec)
#define T8 /* 0xfd469501 */ (T_MASK ^ 0x02b96afe)
#define T9    0x698098d8
#define T10 /* 0x8b44f7af */ (T_MASK ^ 0x74bb0850)
#define T11 /* 0xffff5bb1 */ (T_MASK ^ 0x0000a44e)
#define T12 /* 0x895cd7be */ (T_MASK ^ 0x76a32841)
#define T13    0x6b901122
#define T14 /* 0xfd987193 */ (T_MASK ^ 0x02678e6c)
#define T15 /* 0xa679438e */ (T_MASK ^ 0x5986bc71)
#define T16    0x49b40821
#define T17 /* 0xf61e2562 */ (T_MASK ^ 0x09e1da9d)
#define T18 /* 0xc040b340 */ (T_MASK ^ 0x3fbf4cbf)
#define T19    0x265e5a51
#define T20 /* 0xe9b6c7aa */ (T_MASK ^ 0x16493855)
#define T21 /* 0xd62f105d */ (T_MASK ^ 0x29d0efa2)
#define T22    0x02441453
#define T23 /* 0xd8a1e681 */ (T_MASK ^ 0x275e197e)
#define T24 /* 0xe7d3fbc8 */ (T_MASK ^ 0x182c0437)
#define T25    0x21e1cde6
#define T26 /* 0xc33707d6 */ (T_MASK ^ 0x3cc8f829)
#define T27 /* 0xf4d50d87 */ (T_MASK ^ 0x0b2af278)
#define T28    0x455a14ed
#define T29 /* 0xa9e3e905 */ (T_MASK ^ 0x561c16fa)
#define T30 /* 0xfcefa3f8 */ (T_MASK ^ 0x03105c07)
#define T31    0x676f02d9
#define T32 /* 0x8d2a4c8a */ (T_MASK ^ 0x72d5b375)
#define T33 /* 0xfffa3942 */ (T_MASK ^ 0x0005c6bd)
#define T34 /* 0x8771f681 */ (T_MASK ^ 0x788e097e)
#define T35    0x6d9d6122
#define T36 /* 0xfde5380c */ (T_MASK ^ 0x021ac7f3)
#define T37 /* 0xa4beea44 */ (T_MASK ^ 0x5b4115bb)
#define T38    0x4bdecfa9
#define T39 /* 0xf6bb4b60 */ (T_MASK ^ 0x0944b49f)
#define T40 /* 0xbebfbc70 */ (T_MASK ^ 0x4140438f)
#define T41    0x289b7ec6
#define T42 /* 0xeaa127fa */ (T_MASK ^ 0x155ed805)
#define T43 /* 0xd4ef3085 */ (T_MASK ^ 0x2b10cf7a)
#define T44    0x04881d05
#define T45 /* 0xd9d4d039 */ (T_MASK ^ 0x262b2fc6)
#define T46 /* 0xe6db99e5 */ (T_MASK ^ 0x1924661a)
#define T47    0x1fa27cf8
#define T48 /* 0xc4ac5665 */ (T_MASK ^ 0x3b53a99a)
#define T49 /* 0xf4292244 */ (T_MASK ^ 0x0bd6ddbb)
#define T50    0x432aff97
#define T51 /* 0xab9423a7 */ (T_MASK ^ 0x546bdc58)
#define T52 /* 0xfc93a039 */ (T_MASK ^ 0x036c5fc6)
#define T53    0x655b59c3
#define T54 /* 0x8f0ccc92 */ (T_MASK ^ 0x70f3336d)
#define T55 /* 0xffeff47d */ (T_MASK ^ 0x00100b82)
#define T56 /* 0x85845dd1 */ (T_MASK ^ 0x7a7ba22e)
#define T57    0x6fa87e4f
#define T58 /* 0xfe2ce6e0 */ (T_MASK ^ 0x01d3191f)
#define T59 /* 0xa3014314 */ (T_MASK ^ 0x5cfebceb)
#define T60    0x4e0811a1
#define T61 /* 0xf7537e82 */ (T_MASK ^ 0x08ac817d)
#define T62 /* 0xbd3af235 */ (T_MASK ^ 0x42c50dca)
#define T63    0x2ad7d2bb
#define T64 /* 0xeb86d391 */ (T_MASK ^ 0x14792c6e)


static void
md5_process(md5_state_t *pms, const md5_byte_t *data /*[64]*/)
{
    md5_word_t
	a = pms->abcd[0], b = pms->abcd[1],
	c = pms->abcd[2], d = pms->abcd[3];
    md5_word_t t;
#if BYTE_ORDER > 0
    /* Define storage only for big-endian CPUs. */
    md5_word_t X[16];
#else
    /* Define storage for little-endian or both types of CPUs. */
    md5_word_t xbuf[16];
    const md5_word_t *X;
#endif

    {
#if BYTE_ORDER == 0
	/*
	 * Determine dynamically whether this is a big-endian or
	 * little-endian machine, since we can use a more efficient
	 * algorithm on the latter.
	 */
	static const int w = 1;

	if (*((const md5_byte_t *)&w)) /* dynamic little-endian */
#endif
#if BYTE_ORDER <= 0		/* little-endian */
	{
	    /*
	     * On little-endian machines, we can process properly aligned
	     * data without copying it.
	     */
	    if (!((data - (const md5_byte_t *)0) & 3)) {
		/* data are properly aligned */
		X = (const md5_word_t *)data;
	    } else {
		/* not aligned */
		memcpy(xbuf, data, 64);
		X = xbuf;
	    }
	}
#endif
#if BYTE_ORDER == 0
	else			/* dynamic big-endian */
#endif
#if BYTE_ORDER >= 0		/* big-endian */
	{
	    /*
	     * On big-endian machines, we must arrange the bytes in the
	     * right order.
	     */
	    const md5_byte_t *xp = data;
	    int i;

#  if BYTE_ORDER == 0
	    X = xbuf;		/* (dynamic only) */
#  else
#    define xbuf X		/* (static only) */
#  endif
	    for (i = 0; i < 16; ++i, xp += 4)
		xbuf[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);
	}
#endif
    }

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

    /* Round 1. */
    /* Let [abcd k s i] denote the operation
       a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + F(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  7,  T1);
    SET(d, a, b, c,  1, 12,  T2);
    SET(c, d, a, b,  2, 17,  T3);
    SET(b, c, d, a,  3, 22,  T4);
    SET(a, b, c, d,  4,  7,  T5);
    SET(d, a, b, c,  5, 12,  T6);
    SET(c, d, a, b,  6, 17,  T7);
    SET(b, c, d, a,  7, 22,  T8);
    SET(a, b, c, d,  8,  7,  T9);
    SET(d, a, b, c,  9, 12, T10);
    SET(c, d, a, b, 10, 17, T11);
    SET(b, c, d, a, 11, 22, T12);
    SET(a, b, c, d, 12,  7, T13);
    SET(d, a, b, c, 13, 12, T14);
    SET(c, d, a, b, 14, 17, T15);
    SET(b, c, d, a, 15, 22, T16);
#undef SET

     /* Round 2. */
     /* Let [abcd k s i] denote the operation
          a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + G(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
     /* Do the following 16 operations. */
    SET(a, b, c, d,  1,  5, T17);
    SET(d, a, b, c,  6,  9, T18);
    SET(c, d, a, b, 11, 14, T19);
    SET(b, c, d, a,  0, 20, T20);
    SET(a, b, c, d,  5,  5, T21);
    SET(d, a, b, c, 10,  9, T22);
    SET(c, d, a, b, 15, 14, T23);
    SET(b, c, d, a,  4, 20, T24);
    SET(a, b, c, d,  9,  5, T25);
    SET(d, a, b, c, 14,  9, T26);
    SET(c, d, a, b,  3, 14, T27);
    SET(b, c, d, a,  8, 20, T28);
    SET(a, b, c, d, 13,  5, T29);
    SET(d, a, b, c,  2,  9, T30);
    SET(c, d, a, b,  7, 14, T31);
    SET(b, c, d, a, 12, 20, T32);
#undef SET

     /* Round 3. */
     /* Let [abcd k s t] denote the operation
          a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + H(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
     /* Do the following 16 operations. */
    SET(a, b, c, d,  5,  4, T33);
    SET(d, a, b, c,  8, 11, T34);
    SET(c, d, a, b, 11, 16, T35);
    SET(b, c, d, a, 14, 23, T36);
    SET(a, b, c, d,  1,  4, T37);
    SET(d, a, b, c,  4, 11, T38);
    SET(c, d, a, b,  7, 16, T39);
    SET(b, c, d, a, 10, 23, T40);
    SET(a, b, c, d, 13,  4, T41);
    SET(d, a, b, c,  0, 11, T42);
    SET(c, d, a, b,  3, 16, T43);
    SET(b, c, d, a,  6, 23, T44);
    SET(a, b, c, d,  9,  4, T45);
    SET(d, a, b, c, 12, 11, T46);
    SET(c, d, a, b, 15, 16, T47);
    SET(b, c, d, a,  2, 23, T48);
#undef SET

     /* Round 4. */
     /* Let [abcd k s t] denote the operation
          a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + I(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
     /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  6, T49);
    SET(d, a, b, c,  7, 10, T50);
    SET(c, d, a, b, 14, 15, T51);
    SET(b, c, d, a,  5, 21, T52);
    SET(a, b, c, d, 12,  6, T53);
    SET(d, a, b, c,  3, 10, T54);
    SET(c, d, a, b, 10, 15, T55);
    SET(b, c, d, a,  1, 21, T56);
    SET(a, b, c, d,  8,  6, T57);
    SET(d, a, b, c, 15, 10, T58);
    SET(c, d, a, b,  6, 15, T59);
    SET(b, c, d, a, 13, 21, T60);
    SET(a, b, c, d,  4,  6, T61);
    SET(d, a, b, c, 11, 10, T62);
    SET(c, d, a, b,  2, 15, T63);
    SET(b, c, d, a,  9, 21, T64);
#undef SET

     /* Then perform the following additions. (That is increment each
        of the four registers by the value it had before this block
        was started.) */
    pms->abcd[0] += a;
    pms->abcd[1] += b;
    pms->abcd[2] += c;
    pms->abcd[3] += d;
}

void
md5_init(md5_state_t *pms)
{
    pms->count[0] = pms->count[1] = 0;
    pms->abcd[0] = 0x67452301;
    pms->abcd[1] = /*0xefcdab89*/ T_MASK ^ 0x10325476;
    pms->abcd[2] = /*0x98badcfe*/ T_MASK ^ 0x67452301;
    pms->abcd[3] = 0x10325476;
}

void
md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes)
{
    const md5_byte_t *p = data;
    int left = nbytes;
    int offset = (pms->count[0] >> 3) & 63;
    md5_word_t nbits = (md5_word_t)(nbytes << 3);

    if (nbytes <= 0)
	return;

    /* Update the message length. */
    pms->count[1] += nbytes >> 29;
    pms->count[0] += nbits;
    if (pms->count[0] < nbits)
	pms->count[1]++;

    /* Process an initial partial block. */
    if (offset) {
	int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

	memcpy(pms->buf + offset, p, copy);
	if (offset + copy < 64)
	    return;
	p += copy;
	left -= copy;
	md5_process(pms, pms->buf);
    }

    /* Process full blocks. */
    for (; left >= 64; p += 64, left -= 64)
	md5_process(pms, p);

    /* Process a final partial block. */
    if (left)
	memcpy(pms->buf, p, left);
}

void
md5_finish(md5_state_t *pms, md5_byte_t digest[16])
{
    static const md5_byte_t pad[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    md5_byte_t data[8];
    int i;

    /* Save the length before padding. */
    for (i = 0; i < 8; ++i)
	data[i] = (md5_byte_t)(pms->count[i >> 2] >> ((i & 3) << 3));
    /* Pad to 56 bytes mod 64. */
    md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);
    /* Append the length. */
    md5_append(pms, data, 8);
    for (i = 0; i < 16; ++i)
	digest[i] = (md5_byte_t)(pms->abcd[i >> 2] >> ((i & 3) << 3));
}

//
//
typedef struct _elem_array_dinamic TElemArrayDynamic;
struct _elem_array_dinamic{
	int key;
	void *item;
};

typedef struct _data_array_dinamic TDataArrayDynamic;
struct _data_array_dinamic{
	int sizeMinimum;
	int size;
	int occup;
	TElemArrayDynamic *elem;

};



static TDataArrayDynamic *initDataArrayDynamic( int size ){
	TDataArrayDynamic *data = malloc(sizeof(TDataArrayDynamic));
	size = (size>0?size:1);

	data->sizeMinimum = size;
	data->size = size;
	data->occup=0;
	data->elem = malloc(sizeof(TElemArrayDynamic)*size);

	return data;
}


static void insertArrayDynamic(TArrayDynamic *array, int key, void *item){
	TDataArrayDynamic *data = array->data;

	if (data->occup == data->size){
		data->size = data->size*2;
		data->elem = realloc(data->elem,data->size*sizeof(TElemArrayDynamic));
	}
	data->elem[data->occup].key = key;
	data->elem[data->occup].item = item;
	data->occup++;
}

static void* removeArrayDynamic(TArrayDynamic *array, int key){
	int i;
	void *item=NULL;
	short status=0;
	TDataArrayDynamic *data = array->data;

	i=0;
	while ( (i<data->occup) && (data->elem[i].key != key) )
			i++;

	if (i < data->occup){
		item =  data->elem[i].item;
		while(i<data->occup-1){
			data->elem[i].key = data->elem[i+1].key;
			data->elem[i].item = data->elem[i+1].item;
			i++;
		}
		status=1;
	}

	data->occup=data->occup-status;
	if ( (data->occup < data->size*0.3) && (data->size > data->sizeMinimum)){
		data->size *= 0.5;
		data->elem = realloc(data->elem,sizeof(TElemArrayDynamic)*data->size);
	}

	return item;
}


static void removeAllArrayDynamic(TArrayDynamic *array){

	TDataArrayDynamic *data = array->data;

	data->occup = 0;

	if ( (data->occup < data->size*0.3) && (data->size > data->sizeMinimum)){
		data->size *= 0.5;
		data->elem = realloc(data->elem,sizeof(TElemArrayDynamic)*data->size);
	}

}


static void* retrievalArrayDynamic(TArrayDynamic *array, int key){
	int i;
	TDataArrayDynamic *data = array->data;

	i=0;
	while(i<data->occup){
		if (data->elem[i].key == key)
			return data->elem[i].item;

		i++;
	}
	return NULL;
}


static void* getElementArrayDynamic(TArrayDynamic *array, int index){
	TDataArrayDynamic *data = array->data;

	if (index >= data->occup)
		return NULL;

	return data->elem[index].item;

}

static int getOccupancyArrayDynamic(TArrayDynamic *array){
	TDataArrayDynamic *data = array->data;

	return data->occup;

}

static void swapArrayDynamic(TArrayDynamic *array, int i, int j){
	TDataArrayDynamic *data = array->data;

	if ( (data->occup < i) || (data->occup < j) ){
		fprintf(stderr, "FATAL:internal.c:error on swap. i or j great than occupancy\n");
		exit(0);
	}

	TElemArrayDynamic aux;
	aux.item = data->elem[i].item;
	aux.key = data->elem[i].key;

	data->elem[i].item = data->elem[j].item;
	data->elem[i].key = data->elem[j].key;

	data->elem[j].item = aux.item;
	data->elem[j].key = aux.key;

}

static void* firstArrayDynamic(TArrayDynamic *array){

	return getElementArrayDynamic(array, 0);

}

static void disposeArrayDynamic(TArrayDynamic *array){
	TDataArrayDynamic *data = array->data;

	free(data->elem);
	free(data);
	free(array);

}


TArrayDynamic *createArrayDynamic( int size ){
	TArrayDynamic *array = malloc(sizeof(TArrayDynamic));

	array->data = initDataArrayDynamic(size);
	array->insert = insertArrayDynamic;
	array->remove = removeArrayDynamic;
	array->removeAll = removeAllArrayDynamic;
	array->retrieval = retrievalArrayDynamic;
	array->first = firstArrayDynamic;
	array->getOccupancy = getOccupancyArrayDynamic;
	array->getElement = getElementArrayDynamic;
	array->swapElement = swapArrayDynamic;
	array->ufree = disposeArrayDynamic;

	return array;
}

typedef struct elem_priority_queue TElemPriorityQueue;
struct elem_priority_queue{
	unsigned long int key;
	void *item;
};

typedef short (* TComparePriorityQueue)(unsigned long int, unsigned long int);
typedef struct _data_priority_queue TDataPriorityQueue;
struct _data_priority_queue{
	TElemPriorityQueue *element;
	unsigned int occupancy;
	unsigned int size;
	TComparePriorityQueue compareIns;
	TComparePriorityQueue compareOrd;
};


static void *firstPriorityQueue(TPriorityQueue *pq){
	TDataPriorityQueue *data = pq->data;
	TElemPriorityQueue *elems = data->element;

	if (data->occupancy > 0)
		return elems[0].item;
	else
		return NULL;
}

static short compareMinimumPriorityQueue(unsigned long int k1, unsigned long int k2){
	return (k1>k2);
}

static short compareMaximumPriorityQueue(unsigned long int k1, unsigned long int k2){
	return (k1<k2);
}

static void swapPriorityQueue(TElemPriorityQueue *elems, unsigned int i, unsigned int j){
	TElemPriorityQueue actual;

	actual.key = elems[i].key;
	actual.item = elems[i].item;

	elems[i].key = elems[j].key;
	elems[i].item = elems[j].item;

	elems[j].key = actual.key;
	elems[j].item = actual.item;

}

static void orderPriorityQueue(TPriorityQueue *pq, unsigned int i){
	TDataPriorityQueue *data = pq->data;
	unsigned int left, right;
	unsigned int ord = i;
	short continuing;

	do{
		continuing = 0;
		left = 2*i+1;
		right = 2*i+2;
		if ( (left < data->occupancy) && data->compareOrd(data->element[left].key, data->element[ord].key) ){
			ord = left;
		}
		if ( (right<data->occupancy) && data->compareOrd(data->element[right].key, data->element[ord].key) ){
			ord = right;
		}

		if (ord!=i){
			swapPriorityQueue(data->element, ord, i);
			i = ord;
			continuing = 1;
		}
	} while(continuing);
}

static void enqueuePriorityQueue(TPriorityQueue *pq, unsigned long int key, void *item){
	TDataPriorityQueue *data = pq->data;

	if (data->occupancy == data->size){
		data->size *=2;
		data->element = realloc(data->element,sizeof(TElemPriorityQueue)*data->size);
	}

	unsigned int parent, i = data->occupancy;
	data->element[i].key = key;
	data->element[i].item = item;
	data->occupancy++;

	short continuing = 1;

	do{

		parent = ( (i-1) / 2 );
		if ( (i != 0) && (parent != i) && (data->compareIns(data->element[parent].key, key) ) ){
			swapPriorityQueue(data->element, parent, i);
			i = parent;
		}else{
			continuing = 0; //false
		}
	}while( continuing );

}

static void *dequeuePriorityQueue(TPriorityQueue *pq){
	void *item;
	TDataPriorityQueue *data = pq->data;

	if (data->occupancy == 0)
		return NULL;

	item = data->element[0].item;
	data->occupancy--;

	if (data->occupancy){
		swapPriorityQueue(data->element,0,data->occupancy);
		orderPriorityQueue(pq, 0);
	}

	return item;
}

static short isEmptyPriorityQueue(TPriorityQueue *pq){
	TDataPriorityQueue *data = pq->data;

	return ((data->occupancy==0)?1:0);
}

static void cleanUpPriorityQueue(TPriorityQueue *pq){
	TDataPriorityQueue *data = pq->data;

	data->occupancy = 0;

}

static unsigned int getSizePriorityQueue(TPriorityQueue *pq){
	TDataPriorityQueue *data = pq->data;
	return data->size;
}

static void ufreePriorityQueue(TPriorityQueue *pq){
	TDataPriorityQueue *data = pq->data;
	free(data->element);
	free(data);
	free(pq);

}


static TPriorityQueue *createPriorityQueue(unsigned int size){
	TPriorityQueue *pq = malloc(sizeof(TPriorityQueue));
	TDataPriorityQueue *data = malloc(sizeof(TDataPriorityQueue));

	data->occupancy=0;
	data->compareIns = NULL;
	data->size = size;
	data->element = malloc(sizeof(TElemPriorityQueue)*size);

	pq->data = data;
	pq->enqueue = enqueuePriorityQueue;
	pq->dequeue = dequeuePriorityQueue;
	pq->ufree = ufreePriorityQueue;
	pq->cleanup = cleanUpPriorityQueue;
	pq->isEmpty = isEmptyPriorityQueue;
	pq->first = firstPriorityQueue;
	pq->getSize = getSizePriorityQueue;

	return pq;

}

TPriorityQueue *createMinimumPriorityQueue(unsigned int size){

	TPriorityQueue *pq = createPriorityQueue(size);
	TDataPriorityQueue *data = pq->data;

	data->compareIns = compareMinimumPriorityQueue;
	data->compareOrd = compareMaximumPriorityQueue;

	return pq;
}


TPriorityQueue *createMaximumPriorityQueue(unsigned int size){

	TPriorityQueue *pq = createPriorityQueue(size);
	TDataPriorityQueue *data = pq->data;

	data->compareIns = compareMaximumPriorityQueue;
	data->compareOrd = compareMinimumPriorityQueue;

	return pq;
}

//struct item{
//	int tempo;
//	int inscricao;
//};
//
//void *createItem(int inscricao, int tempo){
//	struct item *t = malloc(sizeof(struct item));
//
//	t->inscricao = inscricao;
//	t->tempo = tempo;
//
//	return t;
//}
//
//int main(){
//	TPriorityQueue *pq = createMinimumPriorityQueue(2);
//	struct item *t;
//	int i;
//
//	printf("Enqueue:\n");
//	for (i=1;i<10;i++){
//		t = createItem(i, i*5);
//		printf("%d %d\n", t->inscricao, t->tempo);
//		enqueuePriorityQueue(pq,i,t);
//	}
//
//	printf("Dequeue:\n");
//
//	for (i=1;i<10;i++){
//		t = dequeuePriorityQueue(pq);
//		printf("%d %d\n", t->inscricao, t->tempo);
//	}
//
//}
