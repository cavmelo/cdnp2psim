/*
 * system.c
 *
 *  Created on: Jul 7, 2010
 *      Author: cesar
 */
#include "system.h"
#include "stdio.h"
#include "stdlib.h"

typedef struct _data_system_info TDataSystemInfo;
struct _data_system_info{
	TTimeSystem currentTime;
	TLengthVideoCatalog lengthVideoCatalog;
};


static TTimeSystem getTimeSystemInfo(TSystemInfo *systemInfo){
	TDataSystemInfo *data = systemInfo->data;

	return data->currentTime;
}

static void setTimeSystemInfo(TSystemInfo *systemInfo, TTimeSystem currentTime){
	TDataSystemInfo *data = systemInfo->data;

	data->currentTime = currentTime;
}

static TLengthVideoCatalog getLengthVideoCatalogSystemInfo(TSystemInfo *systemInfo){
	TDataSystemInfo *data = systemInfo->data;

	return data->lengthVideoCatalog;
}

static void setLengthVideoCatalogSystemInfo(TSystemInfo *systemInfo, TLengthVideoCatalog lengthVideoCatalog){
	TDataSystemInfo *data = systemInfo->data;

	data->lengthVideoCatalog = lengthVideoCatalog;
}

TSystemInfo* createSystemInfo( TLengthVideoCatalog lengthVideoCatalog ){
	TSystemInfo *sd;
	TDataSystemInfo *data;

	sd = (TSystemInfo *) malloc(sizeof(TSystemInfo));
	data = malloc(sizeof(TDataSystemInfo));

	data->currentTime = 0;
	data->lengthVideoCatalog = lengthVideoCatalog;

	sd->data = data;
	sd->getTime = getTimeSystemInfo;
	sd->setTime = setTimeSystemInfo;
	sd->getLengthVideoCatalog = getLengthVideoCatalogSystemInfo;
	sd->setLengthVideoCatalog = setLengthVideoCatalogSystemInfo;

	return sd;
}
