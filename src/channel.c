/*
 * channel.c
 *
 *  Created on: 07/09/2013
 *      Author: cesar
 */
#include "stdlib.h"
#include "dictionary.h"
#include "internals.h"

#include "channel.h"

typedef struct socket_data_channel{
	int idPeerSrc;
	int idPeerDst;
} TSocketDataChannel;

static TSocketDataChannel *createSocketDataChannel(int idPeerSrc, int idPeerDst ){
	TSocketDataChannel *socket = malloc(sizeof(TSocketDataChannel));

	socket->idPeerSrc = idPeerSrc;
	socket->idPeerDst = idPeerDst;

	return socket;
}

typedef struct _ongoing_data_channel{
	float eb; // effective Bandwidth
	TSocketDataChannel sock;
} TOngoingDataChannel;

TOngoingDataChannel *createOngoingDataChannel(float rate, int idPeerSrc, int idPeerDst){
	TOngoingDataChannel *data = malloc(sizeof(TOngoingDataChannel));

	data->eb = rate;
	data->sock.idPeerSrc = idPeerSrc;
	data->sock.idPeerDst = idPeerDst;

	return data;
}

typedef struct _data_channel{
	float capacity; // Mbps
	float max_uplink;
	float max_downlink;
	float rate_uplink; // Mbps
	float rate_downlink; // Mbps
	float prefetchDownlinkRatePercent; //Percent: 0 to 1
	TDictionary *ongoingUL; // opened data channel on UPLink
	TDictionary *ongoingDL; // opened data channel on DOWNLink

} TDataChannel;


static TDataChannel *initDataChannel(float capacity, float rate_uplink, float prefetchDownlinkRatePercent){
	TDataChannel *data = malloc(sizeof(TDataChannel));

	data->capacity =capacity;
	data->max_uplink = rate_uplink;
	data->max_downlink = capacity - rate_uplink;
	data->rate_uplink = rate_uplink;
	data->rate_downlink = data->max_downlink;
	data->prefetchDownlinkRatePercent = prefetchDownlinkRatePercent;
	data->ongoingUL = createDictionary();
	data->ongoingDL = createDictionary();
	return data;
}

/*bandwidthBroker(){

}*/

enum {UPLINK = 1, DOWNLINK=2, UNDEFINED=3};

static short canStreamDataChannel(TChannel *channel, float rate){
	TDataChannel *data = channel->data;

	return data->rate_uplink > rate ? 1 : 0;
}

static float getULRateChannel(TChannel *channel){
	TDataChannel *data = channel->data;

	return data->max_uplink - data->rate_uplink;
}

static float getDLRateChannel(TChannel *channel){
	TDataChannel *data = channel->data;

	return data->max_downlink - data->rate_downlink;
}

static short hasDownlinkChannel(TChannel *channel) {
	TDataChannel *data = channel->data;

	return data->rate_downlink > 0 ? 1 : 0;
}

static void closeDLDataChannel(TChannel *channel, unsigned int idPeerDst){
	TOngoingDataChannel *ongoingDC;
	TDataChannel *data = channel->data;

	ongoingDC = data->ongoingDL->remove(data->ongoingDL,idPeerDst);
	data->rate_downlink += ongoingDC->eb;


	free(ongoingDC);
}


static void closeULDataChannel(TChannel *channel, unsigned int idPeerDst){
	TOngoingDataChannel *ongoingDC;
	TDataChannel *data = channel->data;

	ongoingDC = data->ongoingUL->remove(data->ongoingUL,idPeerDst);
	data->rate_uplink += ongoingDC->eb;

	free(ongoingDC);
}

static short openULDataChannel(TChannel *channel, int idPeerSrc, int idPeerDst, float eb, int prefetch){
	TDataChannel *data = channel->data;
	short opened = 0; // status of requested data channel

	if (prefetch)
		eb *= 1.f + data->prefetchDownlinkRatePercent;

	if (data->rate_uplink >= eb){
		TOngoingDataChannel *ongDC = createOngoingDataChannel(eb, idPeerSrc, idPeerDst);
		data->ongoingUL->insert(data->ongoingUL, idPeerDst, ongDC);
		data->rate_uplink -= eb;
		opened = 1; // true
	}else{
		printf("Failed to open UL channel!\n");
		printf("eb: %f, rate_uplink: %f\n", eb, data->rate_uplink);
		fflush(stdout);
	}

	return opened;
}


static short openDLDataChannel(TChannel *channel, int idPeerSrc, int idPeerDst, float eb, int prefetch){
	TDataChannel *data = channel->data;
	short opened=0; // status of requested data channel

	if (prefetch)
		eb *= 1.f + data->prefetchDownlinkRatePercent;

	if( (data->rate_downlink>= eb) ){
		TOngoingDataChannel *ongoingDC = createOngoingDataChannel(eb, idPeerSrc, idPeerDst);
		data->ongoingDL->insert(data->ongoingDL, idPeerDst, ongoingDC);
		data->rate_downlink -= eb;
		opened = 1; // true
	}else{
		printf("Failed to open DL channel!\n");
		printf("eb: %f, rate_downlink: %f\n", eb, data->rate_downlink);
		fflush(stdout);
	}

	return opened;
}


TChannel *createDataChannel(float capacity, float rate_upload, float prefetchDownlinkRatePercent){

	TChannel *channel = malloc(sizeof(TChannel));

	channel->data = initDataChannel(capacity, rate_upload, prefetchDownlinkRatePercent);

	channel->canStream = canStreamDataChannel;
	channel->getULRate = getULRateChannel;
	channel->getDLRate = getDLRateChannel;
	channel->closeDL = closeDLDataChannel;
	channel->openDL = openDLDataChannel;
	channel->closeUL = closeULDataChannel;
	channel->openUL = openULDataChannel;
	channel->hasDownlink = hasDownlinkChannel;

	return channel;
}

/*void destroyDataChannel(TChannel *channel){

	//TDataChannel *data = channel->data;


}*/
