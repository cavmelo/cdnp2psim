/*
 * datasource.h
 *
 *  Created on: Jul 2, 2012
 *      Author: cesar
 */

#ifndef DATASOURCE_H_
#define DATASOURCE_H_

//Catalog Object related definition
// PlayList
typedef struct playlist TPlaylist;
typedef void TSetList;


typedef struct _dataCatalog TDataCatalog;
// Methods
typedef void(* TDisposeDataCatalog)(TDataCatalog *_dataCatalog);
struct _dataCatalog{
	void *data;
	void *dynamic;
	TDisposeDataCatalog dispose;
};


typedef struct _pars_DataCatalog TParsDataCatalog;
TParsDataCatalog* createParsDataCatalog(char *entry, void *randomic);

typedef TDataCatalog *(* TCreateDataCatalog)(TParsDataCatalog *pars);

TDataCatalog *createFromCollectionDataCatalog(TParsDataCatalog *pars);
TDataCatalog *createFromCollectionSingletonDataCatalog(TParsDataCatalog *pars);
void disposeFromCollectionDataCatalog(TDataCatalog *dataCatalog);

TDataCatalog *createFromPlaylistDataCatalog(TParsDataCatalog *pars);
void disposeFromPlaylistDataCatalog(TDataCatalog *dataCatalog);
TDataCatalog *createFromPlaylistSingletonDataCatalog(TParsDataCatalog *pars);
void disposeFromPlaylistSingletonDataCatalog(TDataCatalog *dataCatalog);


// Data source catalog
typedef void*(* TCreateDataSource)(TDataCatalog *);
typedef void TLogDataSource;

void *createDataSourceSymTable();
TCreateDataSource retrivalCallerLogDataSource(TLogDataSource *log, char *pattern);
void disposeLogDataSource(TLogDataSource *log);


//data Source related definitions
typedef struct datasource TDataSource;
typedef void* (* TPickDataSource)(TDataSource *);
typedef void (* TResetDataSource)(TDataSource *);
typedef int (* TSizeDataSource)(TDataSource *);
typedef int (* TDurationDataSource)(TDataSource *);
typedef int (* TFirstkDurationDataSource)(TDataSource *, int k);
struct datasource{
	//private data
	void *datacatalog;

	// public methos
	TPickDataSource pick;
	TResetDataSource reset;
	TSizeDataSource size;
	TDurationDataSource duration;
	TFirstkDurationDataSource firstkduration;
};

//from collection
void *createFromCollectionDataSource(TDataCatalog *dataCatalog);
void *pickFromCollectionDataSource(TDataSource *dataSource);
void resetFromCollectionDataSource(TDataSource *dataSource);
int sizeFromCollectionDataSource(TDataSource *dataSource);
int durationFromCollectionDataSource(TDataSource *dataSource);
int firstkDurationFromCollectionDataSource(TDataSource *dataSource, int k);

void *pickTopKFromCatalogDataSource(TDataSource *dataSource, float ratio);

// Data Source from playlist
void *createFromPlaylistDataSource(TDataCatalog *dataCatalog);
void *pickFromPlaylistDataSource(TDataSource *dataSource);
void resetFromPlaylistDataSource(TDataSource *dataSource);
int sizeFromPlaylistDataSource(TDataSource *dataSource);
int durationFromPlaylistDataSource(TDataSource *dataSource);
int firstkDurationFromPlaylistDataSource(TDataSource *dataSource, int k);

void *pickDataSource(TDataSource *dataSource);
void disposeDataSource(TDataSource *dataSource);

#endif /* DATASOURCE_H_ */
