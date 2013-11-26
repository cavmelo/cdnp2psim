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
	float rate_uplink; // Mbps
	float rate_downlink; // Mbps
	TDictionary *ongoingUL; // opened data channel on UPLink
	TDictionary *ongoingDL; // opened data channel on DOWNLink

} TDataChannel;


static TDataChannel *initDataChannel(float capacity, float rate_uplink){
	TDataChannel *data = malloc(sizeof(TDataChannel));

	data->capacity =capacity;
	data->rate_uplink = rate_uplink;
	data->rate_downlink = capacity - rate_uplink;
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

static void closeDLDataChannel(TChannel *channel, unsigned int idPeerDst){
	TOngoingDataChannel *ongoingDC;
	TDataChannel *data = channel->data;

	ongoingDC = data->ongoingDL->remove(data->ongoingDL,idPeerDst);
	data->rate_downlink += ongoingDC->eb;


	free(ongoingDC);
}


static void closeULDataChannel(TChannel *channel, unsigned int idPeerSrc){
	TOngoingDataChannel *ongoingDC;
	TDataChannel *data = channel->data;

	ongoingDC = data->ongoingUL->remove(data->ongoingUL,idPeerSrc);
	data->rate_uplink += ongoingDC->eb;

	free(ongoingDC);
}

static short openULDataChannel(TChannel *channel, int idPeerSrc, int idPeerDst, float eb){
	TDataChannel *data = channel->data;
	short opened = 0; // status of requested data channel

	if (data->rate_uplink >= eb){
		TOngoingDataChannel *ongDC = createOngoingDataChannel(eb, idPeerSrc, idPeerDst);
		data->ongoingUL->insert(data->ongoingUL, idPeerSrc, ongDC);
		data->rate_uplink -= eb;
		opened = 1; // true
	}

	return opened;
}


static short openDLDataChannel(TChannel *channel, int idPeerSrc, int idPeerDst, float eb){
	TDataChannel *data = channel->data;
	short opened=0; // status of requested data channel

	if( (data->rate_downlink>= eb) ){
		TOngoingDataChannel *ongoingDC = createOngoingDataChannel(eb, idPeerSrc, idPeerDst);
		data->ongoingDL->insert(data->ongoingDL, idPeerDst, ongoingDC);
		data->rate_downlink -= eb;
		opened = 1; // true
	}
	return opened;
}


TChannel *createDataChannel(float capacity, float rate_upload){

	TChannel *channel = malloc(sizeof(TChannel));

	channel->data = initDataChannel(capacity, rate_upload);

	channel->canStream = canStreamDataChannel;
	channel->closeDL = closeDLDataChannel;
	channel->openDL = openDLDataChannel;
	channel->closeUL = closeULDataChannel;
	channel->openUL = openULDataChannel;


	return channel;
}

/*void destroyDataChannel(TChannel *channel){

	//TDataChannel *data = channel->data;


}*/
