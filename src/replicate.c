/*
 * replicate.c
 *
 *  Created on: Sep 18, 2011
 *      Author: cesar
 */
#include "stdio.h"
#include "stdlib.h"
#include "randomic.h"
#include "cache.h"
#include "system.h"
#include "hash.h"
#include "peer.h"
#include "object.h"

#include "replicate.h"


// Policy related data/function definition
//
struct replicate{
	TPolicyReplicate policy;
	TRandomic *cycle;

    void *data;
};


void *getCycleReplicate(TReplicate *replicate){


	return replicate->cycle;
}

TPolicyReplicate getPolicyReplicate(TReplicate *replicate){

	return replicate->policy;
}

static void setDATAReplicate(TReplicate *replicate, TDATAReplicate *data){

	replicate->data = data;
}

static TDATAReplicate *getDATAReplicate(TReplicate *replicate){

	return replicate->data;
}


short RunReplicate(TReplicate *replicate, void *cache, void* systemData, void* hashTable, void** peers){
    short status=0;

    TPolicyReplicate policy = getPolicyReplicate(replicate);

    status = policy(replicate, cache, systemData, hashTable, peers);

    // if (status)
    //    printf("Well done!\n");
    //else
    //    printf("Got problems to insert\n");


    return status;
}


typedef struct DATAReplicateRandomic TDATAReplicateRandomic;

struct DATAReplicateRandomic{
    TListNeighborsReplicate listNeighbors;
    int numNeighbors;
    int maxReplicas;
    int topK;
};

static TDATAReplicateRandomic *initDataReplicateRandomic(int numNeighbors, int *listNeighbors, int maxReplicas, int topK){
	TDATAReplicateRandomic * data = malloc(sizeof(TDATAReplicateRandomic));

	data->listNeighbors=listNeighbors;
	data->maxReplicas=maxReplicas;
	data->numNeighbors=numNeighbors;
	data->topK=topK;

	return data;
}

static void destroyDataReplicateRandomic(TDATAReplicateRandomic *data){
	free(data);
}

static short policyReplicateRandomic(TReplicate* replicate, void* cache, void* systemData, void* hashTable, void **peers){
	return 1;
}


void *initReplicateRandomic(short peer, int numNeighbors, int range, void *cycle, int maxReplicas, int topK){
	int i,j;
	int *listNeighbors;
	short status;
	short neighbor;
	TReplicate *replicate;
	char entry[20];

	sprintf(entry,"%d;",range);
    TRandomic *neighbors = NULL; // initPickRandomic("UNIFORM",entry);

	listNeighbors = (int *)malloc(sizeof(int)*numNeighbors);

	// get a list of neighbors peers
	for(i=0;i<numNeighbors;i++){
		do{
			neighbor = neighbors->pick(neighbors) - 1;
			if (neighbor == peer)
				status = 1;
			else{
				j=0;
				while ( (j<i) && (listNeighbors[j] != neighbor) ){
					j++;
				}
				status = (j<i?1:0); // picked peer is in neighbor list
			}

		}while( status );
		listNeighbors[i] = neighbor;
	}

	replicate = (TReplicate *)malloc(sizeof(TReplicate));

	replicate->policy = policyReplicateRandomic;

	replicate->cycle = cycle;

	replicate->data = initDataReplicateRandomic(numNeighbors, listNeighbors, maxReplicas, topK);

	// dispose TRandomic
	neighbors->dispose(neighbors);

	return replicate;
}


void destroyReplicateRandomic(TReplicate* replicate){

	destroyDataReplicateRandomic(replicate->data);
	disposeRandomic(replicate->cycle);
	free(replicate);
}



int getTopKReplicateRandomic(TReplicate *replicate){
	TDATAReplicateRandomic *data = (TDATAReplicateRandomic*)replicate->data;

	return data->topK;

}

int getNumNeighborsReplicateRandomic(TReplicate *replicate){
	TDATAReplicateRandomic *data = (TDATAReplicateRandomic*)replicate->data;

	return data->numNeighbors;

}

TListNeighborsReplicate getListNeighborsReplicateRandomic(TReplicate *replicate){
	TDATAReplicateRandomic *data = (TDATAReplicateRandomic*)replicate->data;

	return data->listNeighbors;

}

int getMaxReplicasReplicateRandomic(TReplicate *replicate){
	TDATAReplicateRandomic *data = (TDATAReplicateRandomic*)replicate->data;

	return data->maxReplicas;
}

typedef struct DATAReplicateTOPK TDATAReplicateTOPK;

struct DATAReplicateTOPK{
    TListNeighborsReplicate listNeighbors;
    int numNeighbors;
    int topK;
};

int getNumNeighborsReplicateTOPK(TReplicate *replicate){
	TDATAReplicateTOPK *data = (TDATAReplicateTOPK*)replicate->data;

	return data->numNeighbors;
}

TListNeighborsReplicate getListNeighborsReplicateTOPK(TReplicate *replicate){
	TDATAReplicateTOPK *data = (TDATAReplicateTOPK*)replicate->data;

	return data->listNeighbors;
}

int getTopKReplicateTOPK(TReplicate *replicate){
	TDATAReplicateTOPK *data = (TDATAReplicateTOPK*)replicate->data;

	return data->topK;
}

void *initReplicateTopK(void *cycle, void *placement, int topK){

	return NULL;
}

// TOP-K policy
short policyReplicateTOPK(TReplicate* replicate, void* cache, void* systemData, void* vHashTable, void **peers) {
	int i, status;
	TPeer *peer;
	unsigned int idPeerNeighbor;
	TObject *object, *cloneObj;
	TCache *replicateCache;
	TIdObject idObject;
	TListObject *listDisposed;
	TItemHashTable *item;
	THashTable *hashTable = vHashTable;

	item = createItemHashTable();

	int numNeighbors = getNumNeighborsReplicateTOPK(replicate);
	TListNeighborsReplicate listNeighbors = getListNeighborsReplicateTOPK(replicate);
	int topK = getTopKReplicateTOPK(replicate);

	TListObject *candidateslistObject = ((TCache *)cache)->firstK(cache, topK); // K = 2

	while (!candidateslistObject->isEmpty(candidateslistObject)) {
		i = 0;

		object = candidateslistObject->getHead(candidateslistObject);
		cloneObj = cloneObject(object);

		status = 0;
		while ((i < numNeighbors)) { //&& (!status)) {

			while ((i < numNeighbors) && (!peer->isUp(peer))){
				i++;
				if (i< numNeighbors)
					peer = peers[listNeighbors[i]];
			}

			if (i < numNeighbors) {

				idPeerNeighbor = listNeighbors[i];
				i++;

				replicateCache = peer->getCache(peer);

				if (replicateCache->isCacheable(replicateCache, cloneObj, systemData)) {

					if (!replicateCache->has(replicateCache, cloneObj)) { // not found

						if (replicateCache->insert(replicateCache, cloneObj, systemData)) {
							status = 1;

							getIdObject(cloneObj, idObject);
							item->set(item, idPeerNeighbor,peers[idPeerNeighbor], idObject,
									cloneObj);

							hashTable->insert(hashTable, item);

							// updating hash table due to evicting that made room for the cached video
							listDisposed = replicateCache->getEvictedObjects(replicateCache);

							hashTable->removeEvictedItens(hashTable, idPeerNeighbor, listDisposed);

						}
					}
				}
			}

			if (status == 1) // a replica was accepted, get new replica and try to put it on other peer
				cloneObj = cloneObject(object);

		}
		if (status == 0) // object was not accepted
			disposeObject(cloneObj);

		candidateslistObject->removeHead(candidateslistObject);

	}

	candidateslistObject->destroy(candidateslistObject);
	item->dispose(item);

	return (short) 1;
}

