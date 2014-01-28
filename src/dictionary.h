/*
 * internals.h
 *
 *  Created on: Jul 6, 2012
 *      Author: cesar
 */

#ifndef INTERNALS_H_
#define INTERNALS_H_

typedef short (*TCompareListGeneral)(void *stored, void* passed);
struct listGeneral{
	void *data;
	TCompareListGeneral compare;

};


typedef struct dictionary TDictionary;

typedef unsigned long long int TKeyDictionary;
typedef void (* TInsertDictionary )(TDictionary *, TKeyDictionary, void *);
typedef void* (* TRetrievalDictionary )(TDictionary *, TKeyDictionary);
typedef void* (* TRetrievalMinimalDictionary )(TDictionary *);
typedef void* (* TRetrievalRootDictionary )(TDictionary *);
typedef TKeyDictionary (* TRetrievalRootKeyDictionary )(TDictionary *);
typedef void* (* TRemoveDictionary )(TDictionary *, TKeyDictionary);
typedef short (* THasDictionary )(TDictionary *, TKeyDictionary);
typedef void (* TUFreeDictionary )(TDictionary *);

typedef TKeyDictionary (* TKeyGenesisDictionary )(char *);

typedef void* (* TIteratorDictionary )(TDictionary *);

struct dictionary {
	// data private
	void* data;

	// methods
	TInsertDictionary insert;
	TRetrievalDictionary retrieval;
	TRetrievalMinimalDictionary minimal;
	TRetrievalRootDictionary first;
	TRetrievalRootKeyDictionary firstKey;
	TRemoveDictionary remove;
	THasDictionary has;
	TKeyGenesisDictionary keyGenesis;
	TUFreeDictionary ufree;
};


TDictionary *createDictionary();

///
//
//
typedef struct multi_dictionary TMultiDictionary;

typedef TKeyGenesisDictionary TKeyGenesisMultiDictionary;
typedef void (* TInsertMultiDictionary )(TMultiDictionary *, TKeyDictionary, void *item);
typedef short (* THasMultiDictionary )(TMultiDictionary *, TKeyDictionary key);
typedef void (* TUFreeMultiDictionary )(TMultiDictionary *);
typedef void* (* TRemoveMultiDictionary)(TMultiDictionary *, TKeyDictionary, void *content);
typedef void* (* TRetrievalMultiDictionary )(TMultiDictionary *, TKeyDictionary key);



struct multi_dictionary {
	// data private
	void* data;

	// methods
	TInsertMultiDictionary insert;
	TRetrievalMultiDictionary retrieval;
	TRemoveMultiDictionary remove;
	THasMultiDictionary has;
	TKeyGenesisMultiDictionary keyGenesis;
	TUFreeMultiDictionary ufree;
};


TMultiDictionary *createMultiDictionary(TCompareListGeneral compare);


// Iterator for a general list
typedef struct iterator TIterator;

typedef void (* TNextIterator )(TIterator *);
typedef void (* TResetIterator )(TIterator *);
typedef void (* TDestroyIterator )(TIterator *);
typedef short (* THasNextIterator )(TIterator *);
typedef short (* THasIterator )(TIterator *);
typedef void* (* TCurrentIterator )(TIterator *);

struct iterator {
	void *data;
	THasIterator has;
	TNextIterator next;
	TResetIterator reset;
	THasNextIterator hasNext;
	TCurrentIterator current;
	TDestroyIterator ufree;
};

TIterator* createIteratorDictionary( TDictionary* d );
TIterator* createKeyIteratorDictionary( TDictionary* d );
void freeIteratorDictionary(TIterator* i);

TIterator* createSoftIteratorDictionary( TDictionary* d );
void freeSoftIteratorDictionary(TIterator* i);

typedef struct listGeneral TListGeneral;
TIterator* createIteratorListGeneral( TListGeneral* l );


#define Error( Str )        FatalError( Str )
#define FatalError( Str )   fprintf( stderr, "%s\n", Str ), exit( 1 )

#endif /* INTERNALS_H_ */
