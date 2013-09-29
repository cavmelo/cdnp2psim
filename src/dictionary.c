/*
 * dictionary.c
 *
 *  Created on: Jan 2, 2013
 *      Author: cesar
 */
/*
 * internals.c
 *
 *  Created on: Jun 20, 2012
 *      Author: cesar
 */

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "internals.h"
#include "dictionary.h"

typedef struct _data_node_dictionary  TNodeDictionary;
typedef struct nodeGeneral TNodeGeneral;


struct nodeGeneral{
	void *item;
	TNodeGeneral* next;
	TNodeGeneral* prev;
};

typedef struct _data_listGeneral TDataListGeneral;
struct _data_listGeneral{
	TNodeGeneral* first;
	TNodeGeneral* current;
	TNodeGeneral* last;
};

static TNodeGeneral* createNodeGeneral(void *item){
	TNodeGeneral* n = malloc(sizeof(TNodeGeneral));
	n->item = item;
	n->next = NULL;
	n->prev = NULL;

	return n;
}


static TListGeneral* createListGeneral( TCompareListGeneral compare){
	TListGeneral* l = malloc(sizeof(TListGeneral));
	TDataListGeneral *data = malloc(sizeof(TDataListGeneral));


	data->first=NULL;
	data->last = NULL;
	data->current=NULL;

	l->data = data;
	l->compare = compare;
	return l;
}

static short isEmptyListGeneral(TListGeneral* l){
	TDataListGeneral *dataLG = l->data;
	return (dataLG->first==NULL?1:0);
}

static void disposeListGeneral(TListGeneral* l){
	TDataListGeneral *dataLG = l->data;

	TNodeGeneral* n;

	if (l==NULL)
		return;

	while(dataLG->first){
		n = dataLG->first;
		dataLG->first = dataLG->first->next;
		free(n->item);
		free(n);
	}

	dataLG->current=dataLG->first=dataLG->last=NULL;

}

static void softDisposeListGeneral(TListGeneral* l){
	TDataListGeneral *dataLG = l->data;

	TNodeGeneral* n;

	if (l==NULL)
		return;

	while(dataLG->first){
		n = dataLG->first;
		dataLG->first = dataLG->first->next;
		free(n);
	}

	dataLG->current=dataLG->first=dataLG->last=NULL;

}

static void freeListGeneral(TListGeneral* l){
	disposeListGeneral(l);

	free(l->data);
	free(l);
}

static void freeSoftListGeneral(TListGeneral* l){
	softDisposeListGeneral(l);

	free(l->data);
	free(l);
}

static void insertTailListGeneral(TListGeneral* l, void *item){
	TDataListGeneral *dataLG = l->data;

	if (dataLG->first==NULL){
		dataLG->first=dataLG->last = createNodeGeneral(item);
	}else{
		dataLG->last->next = createNodeGeneral(item);
		dataLG->last->next->prev = dataLG->last;
		dataLG->last = dataLG->last->next;
	}
}


// removes the node but keeps the item data
//
static void* removeListGeneral(TListGeneral* l, void* givenItem){
	TDataListGeneral *dataLG = l->data;
	void *item=NULL;
	TNodeGeneral *walker = dataLG->first;

	if (dataLG->first==NULL)
		return NULL;

	while ((walker) && (l->compare(walker->item, givenItem )!=0))
		walker = walker->next;

	if (!walker)
		return NULL;

	if(dataLG->first==walker){
		dataLG->first=dataLG->first->next;
		if (dataLG->first == NULL)
			dataLG->last=NULL;
		else
			dataLG->first->prev = NULL;
	}else if(dataLG->last==walker){
		dataLG->last = dataLG->last->prev;
		dataLG->last->next = NULL;
	}else{
		walker->next->prev = walker->prev;
		walker->prev->next = walker->next;
	}

	item = walker->item;

	if (dataLG->current == walker)
		dataLG->current = walker->next;

	free(walker);

	return item;
}

typedef struct _data_iterator TDataIterator;
struct _data_iterator{
	void *holder;
	TNodeGeneral *current;
};

static void nextIterator(TIterator *it){
	TDataIterator *data = it->data;

	if (data->current!=NULL){
		data->current = data->current->next;
	}

}


static void* currentIterator(TIterator *it){
	void* item=NULL;
	TDataIterator *data = it->data;

	if (data->current!=NULL){
		item = data->current->item;
	}

	return item;
}

static void resetIterator(TIterator *it){
	TDataIterator *data = it->data;
	TListGeneral *l = (TListGeneral *)data->holder;
	TDataListGeneral *dataLG = l->data;

	data->current = dataLG->first;
}


static short hasIterator(TIterator *it){
	if (it==NULL)
		return 0;

	TDataIterator *data = it->data;

	return (data->current==NULL?0:1);

}


static short hasNextIterator(TIterator *it){
	if (it==NULL)
		return 0;

	TDataIterator *data = it->data;

	if (data->current)
		return (data->current->next==NULL?0:1);

	return 0;

}

static void freeIteratorListGeneral( TIterator* it ){
	free(it->data);
	free(it);
}


TIterator* createIteratorListGeneral( TListGeneral* l ){
	TIterator *it = malloc(sizeof(TIterator));
	TDataIterator *data = malloc(sizeof(TDataIterator));
	TDataListGeneral *dataLG = l->data;
	//initList();

	data->holder = l;
	data->current = dataLG->first;

	it->data = data;
	it->next = nextIterator;
	it->hasNext = hasNextIterator;
	it->has = hasIterator;
	it->reset = resetIterator;
	it->ufree = freeIteratorListGeneral;
	it->current = currentIterator;

	return it;
}


//
// data and function related to Dictionary
struct _data_node_dictionary {
	void *content;
	TKeyDictionary key;
	TNodeDictionary *left;
	TNodeDictionary *right;
	int height;
};

typedef struct _data_dictionary TDataDictionary;
struct _data_dictionary{
	void *root;
};

static void IteratorAVL(TNodeDictionary* T, TListGeneral* l){
	if (T==NULL)
		return;

	IteratorAVL(T->left, l);
	insertTailListGeneral(l,T->content);
	IteratorAVL(T->right, l);

}

static void *freeAVL(TNodeDictionary* T){
	if (T==NULL){
		return NULL;
	}
	freeAVL(T->left);
	freeAVL(T->right);
	free(T->content);free(T);
	return NULL;
}

static TNodeDictionary* findAVL( TKeyDictionary X, TNodeDictionary* T )
{
	if( T == NULL ){
		return NULL;
	}if( X < T->key )
		return findAVL( X, T->left );
	else
		if( X > T->key )
			return findAVL( X, T->right );
		else
			return T;
}

static TNodeDictionary* findMinAVL( TNodeDictionary* T )
{
	if( T == NULL )
		return NULL;
	else
		if( T->left == NULL )
			return T;
		else
			return findMinAVL( T->left );
}


static int heightAVL( TNodeDictionary* P )
{
	if( P == NULL )
		return -1;
	else
		return P->height;
}

static void* rootAVL( TNodeDictionary* P )
{
	if (P == NULL)
		return P;
	else
		return P->content;
}


static int maxAVL( int Lhs, int Rhs )
{
	return Lhs > Rhs ? Lhs : Rhs;
}


/* This function can be called only if K2 has a left child */
/* Perform a rotate between a node (K2) and its left child */
/* Update heights, then return new root */

static TNodeDictionary* singleRotateWithLeftAVL( TNodeDictionary* K2 )
{
	TNodeDictionary* K1;

	K1 = K2->left;
	K2->left = K1->right;
	K1->right = K2;

	K2->height = maxAVL( heightAVL( K2->left ), heightAVL( K2->right ) ) + 1;
	K1->height = maxAVL( heightAVL( K1->left ), K2->height ) + 1;

	return K1;  /* New root */
}


/* This function can be called only if K1 has a right child */
/* Perform a rotate between a node (K1) and its right child */
/* Update heights, then return new root */

static TNodeDictionary* singleRotateWithRightAVL( TNodeDictionary* K1 )
{
	TNodeDictionary* K2;

	K2 = K1->right;
	K1->right = K2->left;
	K2->left = K1;

	K1->height = maxAVL( heightAVL( K1->left ), heightAVL( K1->right ) ) + 1;
	K2->height = maxAVL( heightAVL( K2->right ), K1->height ) + 1;


	return K2;  /* New root */
}


/* This function can be called only if K3 has a left */
/* child and K3's left child has a right child */
/* Do the left-right double rotation */
/* Update heights, then return new root */

static TNodeDictionary* doubleRotateWithLeftAVL( TNodeDictionary* K3 )
{
	/* Rotate between K1 and K2 */
	K3->left = singleRotateWithRightAVL( K3->left );

	/* Rotate between K3 and K2 */
	return singleRotateWithLeftAVL( K3 );
}


/* This function can be called only if K1 has a right */
/* child and K1's right child has a left child */
/* Do the right-left double rotation */
/* Update heights, then return new root */

static TNodeDictionary* doubleRotateWithRightAVL( TNodeDictionary* K1 )
{
	/* Rotate between K3 and K2 */
	K1->right = singleRotateWithLeftAVL( K1->right );

	/* Rotate between K1 and K2 */
	return singleRotateWithRightAVL( K1 );
}



static TNodeDictionary* insertAVL( TKeyDictionary X, TNodeDictionary* T, void *content )
{
	if( T == NULL )
	{
		/* Create and return a one-node tree */
		T = malloc( sizeof( TNodeDictionary ) );
		if( T == NULL )
			FatalError( "Out of space!!!" );
		else
		{
			T->key = X; T->height = 0;
			T->left = T->right = NULL;
			T->content = content;
		}
	}
	else
		if( X < T->key )
		{
			T->left = insertAVL( X, T->left, content );
			if( heightAVL( T->left ) - heightAVL( T->right ) == 2 ){
				if( X < T->left->key )
					T = singleRotateWithLeftAVL( T );
				else
					T = doubleRotateWithLeftAVL( T );
			}
		}
		else
			if( X > T->key )
			{
				T->right = insertAVL( X, T->right, content );
				if( heightAVL( T->right ) - heightAVL( T->left ) == 2 ){
					if( X > T->right->key )
						T = singleRotateWithRightAVL( T );
					else
						T = doubleRotateWithRightAVL( T );
				}
			}else{
				FatalError( "Duplicate key on UNIQUE dictionary!!!" );
			}
	/* Else X is in the tree already; we'll do nothing */

	T->height = maxAVL( heightAVL( T->left ), heightAVL( T->right ) ) + 1;
	return T;
}


static TNodeDictionary* deleteAVL( TKeyDictionary X, TNodeDictionary* T, void **content )
{
	if (T == NULL)
		return NULL;

	else if (T->key > X){
		T->left = deleteAVL(X, T->left, content);
		if( heightAVL( T->right ) - heightAVL( T->left ) == 2 ){
			int fb = heightAVL( T->right->right ) - heightAVL( T->right->left );
			if( fb != -1 )
				T = singleRotateWithRightAVL( T );
			else
				T = doubleRotateWithRightAVL( T );
		}
		T->height = maxAVL( heightAVL( T->left ), heightAVL( T->right ) ) + 1;
	}else if (T->key < X){
		T->right = deleteAVL(X, T->right, content);
		if( heightAVL( T->left ) - heightAVL( T->right ) == 2 ){
			int fb = heightAVL( T->left->left ) - heightAVL( T->left->right );
			if( fb != -1 )
				T = singleRotateWithLeftAVL( T );
			else
				T = doubleRotateWithLeftAVL( T );
		}
		T->height = maxAVL( heightAVL( T->left ), heightAVL( T->right ) ) + 1;
	}else { /* achou o elemento */
		TNodeDictionary* R;

		if (T->left == NULL && T->right == NULL){
			*content = T->content; free(T); T=NULL;
		}else if (T->left == NULL){
			R=T;*content = T->content;T=T->right; free(R);
		}else if (T->right == NULL){
			R=T;*content = T->content;T=T->left;free(R);
		}else {

			R = findMinAVL(T->right);

			TKeyDictionary key = T->key;
			void *localContent = T->content;

			T->key = R->key;
			T->content = R->content;

			R->key = key;
			R->content = localContent;

			T->right = deleteAVL(X, T->right, content);
			if( heightAVL( T->left ) - heightAVL( T->right ) == 2 ){
				int fb = heightAVL( T->left->left ) - heightAVL( T->left->right );
				if( fb != -1 )
					T = singleRotateWithLeftAVL( T );
				else
					T = doubleRotateWithLeftAVL( T );
			}
			T->height = maxAVL( heightAVL( T->left ), heightAVL( T->right ) ) + 1;
		}

	}
	return T;
}



static void insertDictionary(TDictionary* dic, TKeyDictionary key, void * content){
	TDataDictionary *data = dic->data;
	data->root = insertAVL(key,data->root, content);

}

static void* removeDictionary(TDictionary* dic, TKeyDictionary key){
	void *content=NULL;

	TDataDictionary *data = dic->data;

	data->root = deleteAVL(key,data->root,&content);

	//free(content);

	return content;
}


static void* retrievalDictionary(TDictionary* dic, TKeyDictionary key){
	TNodeDictionary *node;
	TDataDictionary *data = dic->data;

	node = findAVL(key, data->root);

	return (node?node->content:NULL);

}

static void* retrievalMinDictionary(TDictionary* dic){
	TNodeDictionary *node;
	TDataDictionary *data = dic->data;

	node = findMinAVL(data->root);

	return (node?node->content:NULL);

}

static void* retrievalFirstDictionary(TDictionary* dic){
	void *content=NULL;
	TDataDictionary *data = dic->data;

	content = rootAVL(data->root);

	return content;

}


static short hasDictionary(TDictionary* dic, TKeyDictionary key){
	TDataDictionary *data = dic->data;

	return (findAVL(key, data->root)?1:0);

}


static TKeyDictionary keyGenesisDictionary(char *key){
	TKeyDictionary keyCode=0;
	int len = strlen((char *)key);

	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	char *stop;
	int di;

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)key, len);
	md5_finish(&state, digest);
	for (di = 0; di < 7; ++di)
		sprintf(hex_output + di * 2, "%02x", digest[di+9]);
	//	printf("MD5 (\"%s\") = \n",hex_output);
	//	for (di = 0; di < 16; ++di)
	//	    sprintf(hex_output + di * 2, "%02x", digest[di]);
	//	printf("MD5 (\"%s\") = \n",hex_output);
	//	exit(0);
	keyCode = strtoll(hex_output,&stop,16);

	return keyCode;
}


void freeDictionary(TDictionary *dic){
	TDataDictionary *data = dic->data;

	freeAVL(data->root);
	free(data);
	free(dic);
}



TDictionary *createDictionary(){
	TDictionary *d = (TDictionary *)malloc(sizeof(TDictionary));
	TDataDictionary *data = malloc(sizeof(TDataDictionary));

	data->root = NULL;
	d->data = data;
	d->insert = insertDictionary;
	d->has = hasDictionary;
	d->remove = removeDictionary;
	d->retrieval = retrievalDictionary;
	d->first = retrievalFirstDictionary;
	d->keyGenesis = keyGenesisDictionary;
	d->ufree = freeDictionary;

	return d;
}


// ITERATOR dictionary
//
static void *initIteratorDictionary(TDictionary *dic){
	TDataDictionary *dataDic = dic->data;
	TListGeneral *l = createListGeneral(NULL);
	TDataListGeneral *dataLG = l->data;

	IteratorAVL(dataDic->root, l);

	dataLG->current = dataLG->first;

	return l;
}

void freeIteratorDictionary( TIterator* it ){
	TDataIterator *data = it->data;
	freeListGeneral(data->holder);

	free(it);
}


void freeSoftIteratorDictionary( TIterator* it ){
	TDataIterator *data = it->data;
	freeSoftListGeneral(data->holder);

	free(it);
}




TIterator* createIteratorDictionary( TDictionary* dic ){
	TIterator *it = malloc(sizeof(TIterator));
	TDataIterator *data = malloc(sizeof(TDataIterator));

	//initList();
	data->holder = initIteratorDictionary(dic);

	it->data = data;
	it->next = nextIterator;
	it->hasNext = hasNextIterator;
	it->reset = resetIterator;
	it->current = currentIterator;
	it->has = hasIterator;
	it->ufree = freeIteratorDictionary;

	return it;
}

TIterator* createSoftIteratorDictionary( TDictionary* dic ){
	TIterator *it = malloc(sizeof(TIterator));
	TDataIterator *data = malloc(sizeof(TDataIterator));

	//initList();
	data->holder = initIteratorDictionary(dic);

	it->data = data;
	it->next = nextIterator;
	it->hasNext = hasNextIterator;
	it->reset = resetIterator;
	it->current = currentIterator;
	it->has = hasIterator;
	it->ufree = freeSoftIteratorDictionary;

	return it;
}

//
//
// MultiDictionary related defintions
//
typedef TCompareListGeneral TCompareMultiDictionary;
typedef struct _data_multidictionary TDataMultiDictionary;

struct _data_multidictionary{
	void *root;
	TCompareMultiDictionary compare;
};

static short hasMultiDictionary(TMultiDictionary* dic, TKeyDictionary key){
	TNodeDictionary *node;
	TDataMultiDictionary *data = dic->data;

	node = findAVL(key, data->root);

	return (node?1:0);
}


static void* retrievalMultiDictionary(TMultiDictionary* dic, TKeyDictionary key){
	TNodeDictionary *node;
	TDataMultiDictionary *data = dic->data;

	node = findAVL(key, data->root);

	return (node!=NULL?node->content:NULL);

}

static void insertMultiDictionary(TMultiDictionary* dic, TKeyDictionary key, void *content){
	TDataMultiDictionary *data = dic->data;
	TListGeneral *holder = retrievalMultiDictionary(dic,key);
	if (holder){
		insertTailListGeneral(holder, content);
	}else{
		holder = createListGeneral(data->compare);
		insertTailListGeneral(holder, content);
		data->root = insertAVL(key,data->root, holder);
	}
}

static void* removeMultiDictionary(TMultiDictionary* dic, TKeyDictionary key, void* item){
	void *content = NULL;
	void *removed = NULL;
	TDataDictionary *data = dic->data;
	TListGeneral *holder = retrievalMultiDictionary(dic,key);

	if (holder){

		removed = removeListGeneral(holder, item);
		if (isEmptyListGeneral(holder)){
			data->root = deleteAVL(key,data->root, &content);

			freeListGeneral(content);
		}

	}

	return removed;
}


static void freeMultiDictionary(TMultiDictionary* dic){
	TDataDictionary *data = dic->data;
	TListGeneral *holder = retrievalMinDictionary((TDictionary*)dic);
	while(holder){
		disposeListGeneral(holder);
		holder = retrievalMinDictionary((TDictionary*)dic);
	}

	freeAVL(data->root);
	free(data);
	free(dic);


}


TMultiDictionary *createMultiDictionary( TCompareMultiDictionary compare){
	TMultiDictionary *d = (TMultiDictionary *)malloc(sizeof(TMultiDictionary));
	TDataMultiDictionary *data = malloc(sizeof(TDataMultiDictionary));
	data->root = NULL;
	data->compare = compare;

	d->data = data;
	d->has = hasMultiDictionary;
	d->retrieval = retrievalMultiDictionary;
	d->keyGenesis = keyGenesisDictionary;

	d->ufree = freeMultiDictionary;
	d->insert = insertMultiDictionary;
	d->remove = removeMultiDictionary;

	return d;
}



//int* create(int valor){
//	int * p = malloc(sizeof(int));
//	*p = valor;
//	return p;
//}
//
//
//short criterio(void *xpassado, void* xarmazenado){
//	int *passado = xpassado;
//	int *armazenado = xarmazenado;
//
//	if (*passado == *armazenado){
//		return 0;
//	}else if(*passado > *armazenado)
//		return 1;
//	else
//		return 2;
//}
//

//#include "randomic.h"
//
//int main(){
//	TRandomic *rp = createUniformRandomic("10000;");
//	TRandomic *ro = createUniformRandomic("2;");
//	TMultiDictionary *d = createMultiDictionary(criterio);
//	//TDictionary *d = createDictionary();
//	int e = 0, *pe;
//	int op;
//	while((op!=3)){
//		//scanf("%d",&op);
//		op = ro->pick(ro);
//		e = rp->pick(rp);
//		printf("Operacao: %d valor : %d \n", op, e );
//		if (op==1){ // insert
//			//scanf("%d",&e);
//			pe = create(e);
//			d->insert(d, e, pe);
//
//		}else if(op==2){ // remove
////			scanf("%d",&e);
//			pe = d->remove(d, e, &e);
//			if (pe)
//				free(pe);
//			//d->remove(d, e);
//		}else if(op==3){
//			//showAVL(d->data);printf("\n");
//		}
//	}
//
//	TIterator *i = createIteratorMultiDictionary(d);
//
//	while(i->hasNext(i)){
//		int *c = i->next(i);
//		printf("%d\n",*c);
//	}
////	i->reset(i);
////
////	while(i->hasNext(i)){
////		int *c = i->next(i);
////		printf("%d\n",*c);
////	}
//
//	//destroyIteratorUniqueDictionary(i);
//
//	//ShowAVL(d->data);printf("\n");
//}




