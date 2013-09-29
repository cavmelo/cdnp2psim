/*
 * replicate.h
 *
 *  Created on: Sep 20, 2011
 *      Author: cesar
 */

#ifndef REPLICATE_H_
#define REPLICATE_H_

typedef struct replicate TReplicate;
typedef void TDATAReplicate;
typedef int * TListNeighborsReplicate;


typedef short (* TPolicyReplicate)(TReplicate* replicate, void* cache, void* systemData, void* hashTable, void** peers);

TPolicyReplicate getPolicyReplicate(TReplicate *replicate);
void *getCycleReplicate(TReplicate *replicate);
short RunReplicate(TReplicate *replicate, void *cache, void* systemData, void *hastTable, void** peers);


// Randomic walk

typedef struct DATAReplicateRandomic TRandomicDATAReplicate;
void *initReplicateRandomic(short peer, int neighbors, int range, void *cycle, int maxReplicas, int topK);
int getNumNeighborsReplicateRandomic(TReplicate *replicate);
TListNeighborsReplicate getListNeighborsReplicateRandomic(TReplicate *replicate);
int getMaxReplicasReplicateRandomic(TReplicate *replicate);
int getTopKReplicateRandomic(TReplicate *replicate);


// TOP-K

#endif /* REPLICATE_H_ */
