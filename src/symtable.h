/*
 * catalog.h
 *
 *  Created on: Jul 27, 2012
 *      Author: cesar
 */

#ifndef CATALOG_H_
#define CATALOG_H_

// Caller-parameters Catalog
typedef struct symTable TSymTable;
typedef void  (* TDisposeCatalog)(TSymTable *);
typedef void* (* TCallerCatalog)(TSymTable *, char *pattern, void *pars);
typedef void  (* TGetCallerCatalog)(TSymTable *, char *pattern, void **caller);
typedef void  (* TGetParameterCatalog)(TSymTable *, char *pattern, char *pars);

struct symTable{
	void *data;
	TCallerCatalog caller;
	TGetCallerCatalog getCaller;
	TGetParameterCatalog getPars;
	TDisposeCatalog dispose;
};

// Table of symbols
//
TSymTable *createSymTable();

#endif /* CATALOG_H_ */
