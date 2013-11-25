#include "stdio.h"
#include "stdlib.h"
#include "replicate.h"
#include "datasource.h"
#include "randomic.h"
#include "cache.h"
#include "object.h"
#include "system.h"
#include "topology.h"
#include "dictionary.h"
#include "peer.h"
#include "channeal.c"


typedef struct profile TProfilePolicyPeer;
typedef float (*TRunProfilePeer)(TPeer *peer, void* profilePolicy);
//typedef void * (*TGetProfilePeer)(TPeer *peer);
struct profile{
	void *data;
	TGetProfilePeer get;
	TRunProfilePeer run;
};

float runJaccardContentProfilePeer(TPeer *peer, void* vRemoteCache){
	TCache *localCache = peer->getCache(peer);
	TCache *remoteCache = vRemoteCache;
	int A = 0,B = 0,C = 0;
	float CoeficienteJaccard = 0;
	TObject *video=NULL;
	TIteratorListObject *it = createIteratorListObject(remoteCache->getObjects(remoteCache));
	it->reset(it);

	video = it->current(it);
	while(video != NULL){
		if(localCache->has(localCache,video)){
			A++;
		}else{
			B++;
		}

		it->next(it); video = it->current(it);
	}
	C =  abs(localCache->getNumberOfStoredObject(localCache) - A);
	CoeficienteJaccard = ((float) A / ((float)A+(float)B+(float)C));

	it->ufree(it);

	return CoeficienteJaccard;
}


void * getJaccardContentProfilePeer(TPeer *peer){

	return  peer->getCache(peer);
}


void *createJaccardContentProfilePeer(void *entry){
	TProfilePolicyPeer *sf = malloc(sizeof(TProfilePolicyPeer));

	sf->data = entry;
	sf->get = getJaccardContentProfilePeer;
	sf->run = runJaccardContentProfilePeer;

	return sf;
}

float runAnderbergContentProfilePeer(TPeer *peer, void *vRemoteCache){
	TCache *localCache = peer->getCache(peer);
	TCache *remoteCache = vRemoteCache;
	int A = 0,B = 0,C = 0;
	float coeficient = 0;
	TObject *video=NULL;
	TIteratorListObject *it = createIteratorListObject(remoteCache->getObjects(remoteCache));
	it->reset(it);

	video = it->current(it);
	while(video != NULL){
		if(localCache->has(localCache,video)){
			A++;
		}else{
			B++;
		}

		it->next(it); video = it->current(it);
	}
	C =  abs(localCache->getNumberOfStoredObject(localCache) - A);
	coeficient = (float)A / (A + (2 * (B+C)));

	it->ufree(it);

	return coeficient;
}


void * getAnderbergContentProfilePeer(TPeer *peer){
	return  peer->getCache(peer);
}

void *createAnderbergContentProfilePeer(void *entry){
	TProfilePolicyPeer *sf = malloc(sizeof(TProfilePolicyPeer));

	sf->data = entry;
	sf->get = getAnderbergContentProfilePeer;
	sf->run = runAnderbergContentProfilePeer;

	return sf;
}


typedef struct sessionLasting TSessionLasting;
typedef unsigned int (*TPickSessionLasting)(TSessionLasting *sl, void*);
struct sessionLasting{
	void *data;
	TPickSessionLasting pick;
};

unsigned int randomPickSessionLasting(TSessionLasting *sl, void* vPeer){
	TRandomic *rand = sl->data;

	return rand->pick(rand);
}

void *createRandomSessionLasting(void *entry){
	TSessionLasting *sl = malloc(sizeof(TSessionLasting));

	sl->data = entry;
	sl->pick = randomPickSessionLasting;

	return sl;
}



unsigned int partialPlaylistPickSessionLasting(TSessionLasting *sl, void* vPeer){
	TRandomic *randomic = sl->data;
	TPeer *peer = vPeer;
    TDataSource *dataSource = peer->getDataSource(peer);
    char xsize[20];
    int size = dataSource->size(dataSource);

    sprintf(xsize, "%d;", size -1);
    randomic->reset(randomic, xsize);
    int boundedSize = randomic->pick(randomic) + 1;
    int duration = dataSource->firstkduration(dataSource, boundedSize);

	return duration;
}

void *createPartialPlaylistSessionLasting(void *entry){
	TSessionLasting *sl = malloc(sizeof(TSessionLasting));

	sl->data = entry;
	sl->pick = partialPlaylistPickSessionLasting;

	return sl;
}



unsigned int playlistPickSessionLasting(TSessionLasting *sl, void* vPeer){
	//TRandomic *rand = sl->data;
	TPeer *peer = vPeer;
    TDataSource *dataSource = peer->getDataSource(peer);

    int size = dataSource->size(dataSource);
    int duration = dataSource->firstkduration(dataSource, size);

	return duration;
}

void *createPlaylistSessionLasting(void *entry){
	TSessionLasting *sl = malloc(sizeof(TSessionLasting));

	sl->data = entry;
	sl->pick = playlistPickSessionLasting;

	return sl;
}

typedef struct {
	unsigned int idPeer;
	unsigned int requests;
	unsigned int hopRequests;
	unsigned int hits;
	unsigned int hopHits;
} TDataMapQuery;

typedef struct {
	void *data;

} TMapQuery;

TDataMapQuery *initDataMapQuery(unsigned int idPeer){
	TDataMapQuery *data = malloc(sizeof(TDataMapQuery));

	data->idPeer = idPeer;
	data->hits = 0;
	data->hopHits =0;
	data->requests=0;
	data->hopRequests=0;

	return data;
}

static void updateHitsMapQuery(TMapQuery *map, int hops){
	TDataMapQuery *data = map->data;
	data->hopHits += hops;
	data->hits++;
}

static void updateRequestsMapQuery(TMapQuery *map, int hops){
	TDataMapQuery *data = map->data;

	data->hopRequests += hops;
	data->requests++;
}

static void showMapQuery(TMapQuery *map){
	if (map){
		TDataMapQuery *data = map->data;
		printf("%u\t%d\t%d\t%d\t%d\n", data->idPeer, data->requests, data->hopRequests, data->hits, data->hopHits);
	}
}


static TMapQuery *createMapQuery(unsigned int idPeer){
	TMapQuery *map = malloc(sizeof(TMapQuery));

	map->data = initDataMapQuery(idPeer);

	return map;
}

//peer related logic and data
//
typedef struct _data_peer TDataPeer;
struct _data_peer{
	unsigned int id;
	short tier;
    short status;
    TDictionary *Queries;
    //TClassPeer class;
    TCache *cache;
    TStatsPeer *stats;
    // Controls Dynamic join time
    void *dynamicJoin;
    // controls Dynamic leave time
    void *dynamicLeave;
    // controls requests from peer
    TRandomic *dynamicRequest;
    // controls peer data source
    TDataSource *dataSource;
    // Replicate policy
    TReplicate *replicate;
    //topology manager
    TTopology *topology;
    //! policies for profile estimation
    TProfilePolicyPeer *profilePolicy;

    //Canal
    TChannel *channel;
};




TDataPeer *initDataPeer(unsigned int id, short tier, void *dynamicJoin, void *dynamicLeave, void *dynamicRequest, void *dataSource, void *replicate, void *cache, void *topology, void *channel){
	TDataPeer *data = malloc(sizeof(TDataPeer));

	data->id = id;
	data->status = DOWN_PEER;
	data->tier = tier;

	data->dynamicJoin = dynamicJoin;
	data->dynamicLeave = dynamicLeave;
	data->dynamicRequest = dynamicRequest;

	data->dataSource = dataSource;
	data->cache = cache;
	data->stats = initStatsPeer();

	data->replicate = replicate;

	data->topology = topology;

	data->Queries = createDictionary();

	//Canal
	data->channel = channel;
	//data->channel = createDataChannel();

	return data;
}


static void* getEvictedFromCachePeer(TPeer *peer){
	TDataPeer *data = peer->data;

	// update cache
	TCache *cache = data->cache;

	return cache->getEvictedObjects(cache);
}

static short insertCachePeer(TPeer *peer, void *vObject, void *vSystemData){
	TObject *video = vObject;
	TSystemInfo *systemData = vSystemData;
	TDataPeer *data = peer->data;
	TCache *cache = data->cache;

	//Record a miss
	TStatsCache *statsCache = cache->getStats(cache);
	statsCache->addMiss(statsCache, 1);
	statsCache->addByteMiss(statsCache, getLengthObject(video) );

	//try to insert missed video
	return (cache->insert( cache, video, systemData ) );


}

static void updateCacheAsServerPeer(TPeer *serverPeer, void *vObject, void *vSystemData){
	TObject *storedVideo, *video = vObject;
	TSystemInfo *systemData = vSystemData;
	TDataPeer *data = serverPeer->data;
	TListObject *listObject;
	TCache *cacheServerPeer;

	cacheServerPeer = data->cache;

	// get stored copy
	listObject = cacheServerPeer->getObjects(cacheServerPeer);
	storedVideo = listObject->getObject(listObject, video);

	//updating peer's stats
	TStatsCache *statsCacheServer = cacheServerPeer->getStats(cacheServerPeer);
	statsCacheServer->addCommunityHit(statsCacheServer, 1);
	statsCacheServer->addByteCommunityHit( statsCacheServer, getStoredObject(storedVideo) );

	statsCacheServer->addByteMiss(statsCacheServer, getLengthObject(storedVideo) - getStoredObject(storedVideo) );

	// update cache
	cacheServerPeer->update(cacheServerPeer, video, systemData);

}

static void updateCachePeer(TPeer *peer, void *vObject, void *vSystemData){
	TObject *storedVideo, *video = vObject;
	TSystemInfo *systemData = vSystemData;
	TDataPeer *data = peer->data;

	// update cache
	TCache *cache = data->cache;

	cache->update(cache, video, systemData);

	// record a hit
	TStatsCache *statsCache = cache->getStats(cache);
	statsCache->addHit( statsCache , 1);

	// get stored copy
	TListObject *listObject = cache->getObjects(cache);
	storedVideo = listObject->getObject(listObject, video);
	if (storedVideo == NULL)
		printf("Nào achou o video\n");

	statsCache->addByteHit( statsCache, getStoredObject(storedVideo) );
	statsCache->addByteMiss( statsCache, getLengthObject(storedVideo) - getStoredObject(storedVideo) );

}

static void setupJoiningPeer(TPeer *peer){
	TDataPeer *data = peer->data;
	TDataSource *dataSource = data->dataSource;

	dataSource->reset(dataSource);

}

static void setTierPeer(TPeer *peer, short tier){
	TDataPeer *data = peer->data;
	data->tier = tier;
}


static short getTierPeer(TPeer *peer){
	TDataPeer *data = peer->data;
	return data->tier;
}

static void setTopologyManagerPeer(TPeer *peer, void *topologyManager){
	TDataPeer *data = peer->data;

	data->topology = topologyManager;
}

static void* getTopologyManagerPeer(TPeer *peer){
	TDataPeer *data = peer->data;

	return data->topology;
}

//Canal
static void setChannelPeer(TPeer *peer, void *channel){
	TDataPeer *data = peer->data;

	data->channel = channel;
}

static void* getChannelPeer(TPeer *peer){
	TDataPeer *data = peer->data;

	return data->channel;
}

static TStatsPeer* getOnStatsPeer(TPeer *peer){
	TDataPeer *data = peer->data;
	return data->stats;
}

static unsigned int getIdPeer(TPeer *peer){
	TDataPeer *data = peer->data;
    return data->id;
}

static short getStatusPeer(TPeer *peer){
	TDataPeer *data = peer->data;
    return data->status;
}

static void setStatusPeer(TPeer *peer, short status){
	TDataPeer *data = peer->data;
    data->status = status;
}


static void setDynamicJoinPeer(TPeer *peer, void *dynamicJoin){
	TDataPeer *data = peer->data;
	data->dynamicJoin = dynamicJoin;
}

static void *getDynamicJoinPeer(TPeer *peer){
	TDataPeer *data = peer->data;
	return data->dynamicJoin;
}

static void setDynamicLeavePeer(TPeer *peer, void *dynamicLeave){
	TDataPeer *data = peer->data;
	data->dynamicLeave = dynamicLeave;
}

static void *getDynamicLeavePeer(TPeer *peer){
	TDataPeer *data = peer->data;
	return data->dynamicLeave;
}

static void setDynamicRequestPeer(TPeer *peer, void *dynamicRequest){
	TDataPeer *data = peer->data;
	data->dynamicRequest = dynamicRequest;
}

static void *getDynamicRequestPeer(TPeer *peer){
	TDataPeer *data = peer->data;
	return data->dynamicRequest;
}


static unsigned int getRequestTimePeer(TPeer* peer){
	TDataPeer *data = peer->data;
	TRandomic *randomic = data->dynamicRequest;

	return randomic->pick(randomic);

}

static unsigned int getReplicateTimePeer(TPeer* peer){
	TDataPeer *data = peer->data;

	TRandomic *cycle = getCycleReplicate(data->replicate);

	return cycle->pick(cycle);

}

static unsigned int getUpSessionDurationPeer(TPeer* peer){
	TDataPeer *data = peer->data;

	TSessionLasting *sl = data->dynamicJoin;


	return sl->pick(sl, peer);
}

static unsigned int getDownSessionDurationPeer(TPeer* peer){
	TDataPeer *data = peer->data;
	TSessionLasting *sl = data->dynamicLeave;


	return sl->pick(sl, peer);
}

static void *getCachePeer(TPeer* peer){
	TDataPeer *data = peer->data;
	return data->cache;
}

static void setCachePeer(TPeer* peer, void *cache){
	TDataPeer *data = peer->data;
	data->cache = cache;
}


static void *getReplicatePeer(TPeer* peer){
	TDataPeer *data = peer->data;
	return data->replicate;
}

static void setDataSourcePeer(TPeer *peer, void *dataSource){
	TDataPeer *data = peer->data;
	data->dataSource = dataSource;
}

static void *getDataSourcePeer(TPeer* peer){
	TDataPeer *data = peer->data;
	return data->dataSource;
}

static short isUpPeer(TPeer* peer){
	TDataPeer *data = peer->data;
	return (data->status == UP_PEER);
}

static short isDownPeer(TPeer* peer){
	TDataPeer *data = peer->data;
	return (data->status == DOWN_PEER);
}

static short hasCachedPeer(TPeer* peer, void *object){
	TDataPeer *data = peer->data;
	TCache *cache = data->cache;

	return cache->has(cache,object);
	//return (data->status == DOWN_PEER);
}

static void* getProfilePeer(TPeer* peer){
	TDataPeer *data = peer->data;
	TProfilePolicyPeer *pp = data->profilePolicy;

	return pp->get(peer);
}

static float runProfilePolicyPeer(TPeer* peer, void *rProfile){
	TDataPeer *data = peer->data;
	TProfilePolicyPeer *pp = data->profilePolicy;

	return pp->run(peer,rProfile);

}

static void setProfilePolicyPeer(TPeer* peer, void *profile){
	TDataPeer *data = peer->data;

	data->profilePolicy = profile;
}

static void updateRequestsMapQueryPeer(TPeer* peer, unsigned int idRequester, short hops){
	TDataPeer *data = peer->data;

	TDictionary *queries = data->Queries;

	TMapQuery *map = queries->retrieval(queries, idRequester);

	if (!map){
		map = createMapQuery(idRequester);
		queries->insert(queries,idRequester,map);
	}
	updateRequestsMapQuery(map,hops);
}

static void updateHitsMapQueryPeer(TPeer* peer, unsigned int idRequester, short hops){
	TDataPeer *data = peer->data;

	TDictionary *queries = data->Queries;

	TMapQuery *map = queries->retrieval(queries, idRequester);

	if (!map){
		map = createMapQuery(idRequester);
		queries->insert(queries,idRequester,map);
	}
	updateHitsMapQuery(map,hops);
}

static void showMapQueryPeer(TPeer* peer){
	TDataPeer *data = peer->data;

	TDictionary *queries = data->Queries;

	TIterator *it = createSoftIteratorDictionary(queries);

	it->reset(it);
	while(it->has(it)){
		TMapQuery *map = it->current(it);
		printf("p:\t%d\t",data->id);showMapQuery(map);
		it->next(it);
	}

	it->ufree(it);
}


TPeer* createPeer(unsigned int id,  short tier, void *dynamicJoin, void *dynamicLeave, void *dynamicRequest, void *dataSource, void *replicate, void *cache, void *topo, void *channel){
    TPeer *p = (TPeer*)malloc(sizeof(TPeer));

    //Canal
    p->data = initDataPeer(id, tier, dynamicJoin, dynamicLeave, dynamicRequest, dataSource, replicate, cache, topo, channel);

    p->getOnStats = getOnStatsPeer;
    p->getId = getIdPeer;
    p->getStatus = getStatusPeer;
    p->getDynamicJoin = getDynamicJoinPeer;
    p->getDynamicLeave = getDynamicLeavePeer;
    p->getDynamicRequest = getDynamicRequestPeer;
    p->getRequestTime = getRequestTimePeer;
    p->getReplicateTime = getReplicateTimePeer;
    p->getUpSessionDuration = getUpSessionDurationPeer;
    p->getDownSessionDuration = getDownSessionDurationPeer;
    p->getCache = getCachePeer;
    p->getReplicate = getReplicatePeer;
    p->getDataSource = getDataSourcePeer;
    p->setStatus = setStatusPeer;
    p->setDynamicJoin = setDynamicJoinPeer;
    p->setDynamicLeave = setDynamicLeavePeer;
    p->setDynamicRequest = setDynamicRequestPeer;
    p->setCache = setCachePeer;
    p->setDataSource = setDataSourcePeer;
    p->isUp = isUpPeer;
    p->isDown = isDownPeer;
    p->setupJoining = setupJoiningPeer;
    p->hasCached = hasCachedPeer;

    p->insertCache = insertCachePeer;
    p->updateCache = updateCachePeer;
    p->updateCacheAsServer = updateCacheAsServerPeer;
    p->getEvictedCache = getEvictedFromCachePeer;

    p->getTopologyManager = getTopologyManagerPeer;
    p->setTopologyManager = setTopologyManagerPeer;

    //Canal
    p->getChannel = getChannelPeer;
    p->setChannel = setChannelPeer;

    p->setTier = setTierPeer;
    p->getTier = getTierPeer;

    p->runProfilePolicy = runProfilePolicyPeer;
    p->getProfile = getProfilePeer;
    p->setProfilePolicy = setProfilePolicyPeer;

    // deal with MapQuery
    p->updateHitsMapQuery = updateHitsMapQueryPeer;
    p->updateRequestsMapQuery = updateRequestsMapQueryPeer;
    p->showMapQuery = showMapQueryPeer;

    return p;
}




//Stats related implementation
struct statsPeer{
	TUpTimeStatsPeer upTime;
	TDownTimeStatsPeer downTime;
	TRequestStatsPeer request;
};



TStatsPeer *initStatsPeer(){
	TStatsPeer *stats;

	stats = (TStatsPeer*)malloc(sizeof(TStatsPeer));

	stats->upTime = 0; // how long it stay up
	stats->downTime = 0; // how long it stay down
	stats->request = 0; // number of dispatched request

	return stats;
}

TRequestStatsPeer getRequestStatsPeer(TStatsPeer *stats){
	return stats->request;
}

TUpTimeStatsPeer getUpTimeStatsPeer(TStatsPeer *stats){
	return stats->upTime;
}

TDownTimeStatsPeer getDownTimeStatsPeer(TStatsPeer *stats){
	return stats->downTime;
}


void setRequestStatPeer(TStatsPeer *stats, TRequestStatsPeer request){
	stats->request = request;
}

void setUpTimeStatsPeer(TStatsPeer *stats, TUpTimeStatsPeer upTime){
	stats->upTime = upTime;
}

void setDownTimeStatsPeer(TStatsPeer *stats, TDownTimeStatsPeer downTime){
	stats->downTime = downTime;
}

void addRequestStatsPeer(TStatsPeer *stats, TRequestStatsPeer amount){
	stats->request += amount;
}

void addUpTimeStatsPeer(TStatsPeer *stats, TUpTimeStatsPeer amount){
	stats->upTime += amount;
}

void addDownTimeStatsPeer(TStatsPeer *stats, TDownTimeStatsPeer amount){
	 stats->downTime += amount;
}



/*
int main(){
    int i;
    unsigned short seed[] = {10,11,12};
    unsigned short seed2[] = {120,130,140};
        
    TPickRandomic *pickDataUpDynamic = initPickedUniform(seed,10);
    TPickRandomic *pickDataDownDynamic = initPickedPoisson(seed,15);
    TPickRandomic *pickDataRequest = initPickedUniform(seed2,12);
    TCache *cache = initCache(200000, LRUCache); 
    TPeer *p = initPeer(10, 1, 0, pickDataUpDynamic, pickDataDownDynamic, pickDataRequest, cache );

    for(i=1;i<=100;i++)
        printf("%d %d %d\n", getOffSessionDurationPeer(p), getOnSessionDurationPeer(p), getRequestTimePeer(p));

    return 0;
}

*/
