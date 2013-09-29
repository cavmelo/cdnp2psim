/*
 * system.h
 *
 *  Created on: Jul 7, 2010
 *      Author: cesar
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

typedef struct system_info TSystemInfo;

typedef unsigned long int TTimeSystem;
typedef unsigned long int TLengthVideoCatalog;

typedef TTimeSystem (*TGetTimeSystemInfo)(TSystemInfo *systemData);
typedef void (*TSetTimeSystemInfo)(TSystemInfo *systemData, TTimeSystem currentTime);
typedef TLengthVideoCatalog (*TGetLengthVideoCatalogSystemInfo)(TSystemInfo *systemData);
typedef void (*TSetLengthVideoCatalogSystemInfo)(TSystemInfo *systemData, TLengthVideoCatalog lengthVideoCatalog);

struct system_info{
	void *data;
	TGetTimeSystemInfo getTime;
	TSetTimeSystemInfo setTime;
	TGetLengthVideoCatalogSystemInfo getLengthVideoCatalog;
	TSetLengthVideoCatalogSystemInfo setLengthVideoCatalog;

};

TSystemInfo* createSystemInfo(TLengthVideoCatalog lengthVideoCatalog);

#endif /* SYSTEM_H_ */
