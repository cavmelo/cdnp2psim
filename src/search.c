/*
 * search.c
 *
 *  Created on: Jan 15, 2013
 *      Author: cesar
 */
#include "internals.h"
#include "community.h"
#include "peer.h"
#include "topology.h"
#include "randomic.h"
#include "search.h"

#include "stdio.h"
#include "stdlib.h"

typedef struct _data_search TDataSearch;
struct _data_search{
	void *data_policy;
};

typedef struct _data_randomwalk_search TDataRandomWalkSearch;
struct _data_randomwalk_search{
	TRandomic *walker;
	short maxTries;
};

// random walk
static void* runRandomWalkSearch(TSearch *search, void *vPeer, void *object ){
	TPeer *peer = vPeer;
	TDataSearch *data = search->data;
	TDataRandomWalkSearch *data_rwalk = data->data_policy;
	TRandomic *walker = data_rwalk->walker;
	unsigned int idRequester=-1, idSource = peer->getId(peer);
	short found = 0, tries=-1;
	do{
		TTopology *topo = peer->getTopologyManager(peer);
		found = 1;

		if (!peer->hasCached(peer,object)){
			TArrayDynamic *neighbors = topo->getNeighbors(topo);
			if (idRequester >= 0){
				topo->updateRequestsStats(topo, idRequester, 1);
				peer->updateRequestsMapQuery(peer,idSource,tries+1);
			}
			idRequester = peer->getId(peer);

			int numNeighbors = neighbors->getOccupancy(neighbors);
			if (numNeighbors){
				char entry[30];sprintf(entry,"%d;",numNeighbors);
				walker->reset(walker, entry);
				int i = walker->pick(walker)-1;
				peer = neighbors->getElement(neighbors,i);
			}else{
				tries = data_rwalk->maxTries+1;
			}
			found = 0;
		}else{
			if (idRequester >= 0){
				topo->updateHitsStats(topo, idRequester, 1);
				peer->updateHitsMapQuery(peer,idSource,tries+1);
			}

		}
		tries++;

	}while( (tries <= data_rwalk->maxTries) && (!found) );

	if (tries > data_rwalk->maxTries){
		peer = NULL;
	}
	return peer;
}

void uFreeRandomWalkSearch(TSearch *search){
	TDataSearch *data = search->data;
	TDataRandomWalkSearch *data_rw = data->data_policy;

	free(data_rw);
	free(data);
	free(search);
}

TSearch *createRandomWalkSearch(char *entry){
	TSearch *search = malloc(sizeof(TSearch));
	TDataSearch *data = malloc(sizeof(TDataSearch));
	TDataRandomWalkSearch *data_rw = malloc(sizeof(TDataRandomWalkSearch));

    // extract parameters from user's entry limited by semi-color
    TParameters *lp = createParameters(entry, PARAMETERS_SEPARATOR);

    lp->iterator(lp);


    // walk through the parameters as setting up
    data_rw->maxTries = atoi(lp->next(lp));

    data_rw->walker = createUniformRandomic("10;");

	data->data_policy = data_rw;

	search->data = data;
	search->run = runRandomWalkSearch;
	search->ufree = uFreeRandomWalkSearch;

	lp->dispose(lp);

	return search;
}

//
typedef struct _data_floading_search TDataFloadingSearch;
struct _data_floading_search{
	int maxlevel;
};

typedef struct item_floading_search TItemFloadingSearch;
struct item_floading_search{
	int level;
	int idRequester;
	void *peer;
};

static TItemFloadingSearch *createItemFloadingSearch(int level, int idRequester, void* peer){
	TItemFloadingSearch *item = malloc(sizeof(TItemFloadingSearch));

	item->level  = level;
	item->peer = peer;
	item->idRequester = idRequester;

	return item;
}

static void* runFloadingSearch(TSearch *search, void* vPeer, void* object){
	TPeer *peer = vPeer;
	TDataSearch *data = search->data;
	TDataFloadingSearch *data_floading = data->data_policy;
	TArrayDynamic *visited = createArrayDynamic(10);
	TPriorityQueue *pq = createMaximumPriorityQueue(10);

	short found = 0, deadend=0;
	int i, occup;
	TItemFloadingSearch *item;
	unsigned int level=0, 	idPeer = peer->getId(peer), idSource = idPeer;
	int idRequester = -1;

	visited->insert(visited, idPeer, peer);
	do{
		TTopology *topo = peer->getTopologyManager(peer);

		if ( (!peer->hasCached(peer,object)) ) {
			TArrayDynamic *neighbors = topo->getNeighbors(topo);
			if (idRequester>=0){
				topo->updateRequestsStats(topo,idRequester,1);
				peer->updateRequestsMapQuery(peer,idSource, level);
			}
			idRequester = peer->getId(peer);
			occup = neighbors->getOccupancy(neighbors);
			for(i=0;i<occup;i++){  // BFS on neighborhood
				peer = neighbors->getElement(neighbors,i);
				idPeer = peer->getId(peer);
				if (!visited->retrieval(visited, idPeer)){
					visited->insert(visited, idPeer, peer);
					pq->enqueue(pq,level+1,createItemFloadingSearch(level+1, idRequester, peer));
				}
			}
			item =pq->dequeue(pq);
			if (item){
				level = item->level;
				peer = item->peer;
				idRequester = item->idRequester;
				free(item);
			}else{
				deadend = 1;
			}

		}else{
			found = 1;
			if (idRequester>=0){
				topo->updateHitsStats(topo,idRequester,1);
				peer->updateHitsMapQuery(peer,idSource, level);
			}
		}

	}while( (!found) && (!deadend) && (level<=data_floading->maxlevel) );

	item =pq->dequeue(pq);
	while(item){
		free(item);
		item =pq->dequeue(pq);
	}
	pq->ufree(pq);

	visited->ufree(visited);

	if (!found)
		peer = NULL;

	return peer;
}

static void ufreeFloadingSearch(TSearch *search){
	TDataSearch *data = search->data;
	TDataFloadingSearch *data_floading = data->data_policy;

	free(data_floading);
	free(data);
	free(search);
}

TSearch *createFloadingSearch(char *entry){
	TSearch *search = malloc(sizeof(TSearch));
	TDataSearch *data = malloc(sizeof(TDataSearch));
	TDataFloadingSearch *data_floading = malloc(sizeof(TDataFloadingSearch));

    TParameters *lp = createParameters(entry, PARAMETERS_SEPARATOR);

    lp->iterator(lp);

	data_floading->maxlevel = atoi(lp->next(lp));

	data->data_policy = data_floading;

	search->data = data;
	search->run = runFloadingSearch;
	search->ufree = ufreeFloadingSearch;

	lp->dispose(lp);

	return search;
}

