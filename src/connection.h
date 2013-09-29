/*
 * connection.h
 *
 *  Created on: Dec 31, 2012
 *      Author: cesar
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_


typedef struct connection_manager TConnManager;
typedef short (* TOpenConnManager )(TConnManager* connManager, unsigned int idPeer, void *peer);
typedef void (* TCloseConnManager )(TConnManager* connManager, unsigned int idPeerTo);
typedef void (* TEvaluateConnManager )(TConnManager* connManager, void *sysInfo);
typedef void (* TShowConnManager )(TConnManager* connManager);
typedef void (* TTearDownConnManager )(TConnManager* connManager);
typedef short (*THasPendingConnManager)(TConnManager *connManager);
typedef short (*TIsConnectedConnManager)(TConnManager *connManager);
typedef unsigned int (*TGetIdOwnerConnManager)(TConnManager *connManager);
typedef void* (*TGetOwnerConnManager)(TConnManager *connManager);
typedef void (*TSetOwnerConnManager)(TConnManager *connManager, void*);
typedef int (*TGetConnectedConnManager)(TConnManager *connManager);
typedef short (*TIsFullConnectedConnManager)(TConnManager *connManager);
typedef void* (*TGetListConnectedConnManager)(TConnManager *connManager);
typedef short (* THasEstablishedConnectionConnManager )(TConnManager* connManager, unsigned int idPeerTo);
typedef short (* THasOnGoingConnectionsConnManager )(TConnManager* connManager);
typedef void (* TUpdateRequestsStatsConnManager)(TConnManager *connManager, unsigned int idPeer, int amount);
typedef void (* TUpdateHitsStatsConnManager)(TConnManager *connManager, unsigned int idPeer, int amount);


struct connection_manager{
	void *data;

	TOpenConnManager openConnection;
	TCloseConnManager closeConnection;
	TTearDownConnManager teardownConnection;
	TEvaluateConnManager evaluateConnections;
	THasEstablishedConnectionConnManager hasEstablishedConnection;
	THasOnGoingConnectionsConnManager hasOnGoingConnections; // not complete established connection
	THasPendingConnManager hasPending;
	TIsConnectedConnManager isConnected; // based on counting the established connections
	TShowConnManager showOngoingConnections;
	TShowConnManager showEstablishedConnections;
	TGetIdOwnerConnManager getIdOwner;
	TGetOwnerConnManager getOwner;
	TSetOwnerConnManager setOwner;
	TGetConnectedConnManager getNumEstablishedConnections;
	TIsFullConnectedConnManager isFullConnected;
	TGetListConnectedConnManager getListEstablishedConnections;

	TUpdateHitsStatsConnManager updateHitsStats;
	TUpdateRequestsStatsConnManager updateRequestsStats;
};


TConnManager * createConnManager(int maxConnections, int maxAttempts, unsigned int idPeer, void* peer);

typedef struct connection TConnection;
typedef unsigned long int (*TGetSinceConnection)(TConnection *conn);
typedef unsigned int (*TGetIdClientConnection)(TConnection *conn);
typedef unsigned int (*TGetIdServerConnection)(TConnection *conn);
typedef short (*TGetStatusConnection)(TConnection *conn);
typedef short (*TGetAttemptsConnection)(TConnection *conn);
typedef void* (*TGetPeerConnection)(TConnection *conn);
typedef void (*TSetIdClientConnection)(TConnection *conn, unsigned int idClient);
typedef void (*TSetIdServerConnection)(TConnection *conn, unsigned int idServer);
typedef void (*TSetStatusConnection)(TConnection *conn, short status);
typedef void (*TSetAttemptsConnection)(TConnection *conn, short attempts);
typedef void (*TSetSinceConnection)(TConnection *conn, unsigned long int since);
typedef void (*TUFreeConnection)(TConnection *conn);
typedef float (*TGetIndexOfSatisfactionStatsConnection)( TConnection *conn, int idPeer );
typedef void (*TUpdateHitsStatsConnection)(TConnection *conn, int idPeer, int amount);
typedef void (*TUpdateRequestsStatsConnection)(TConnection *conn, int idPeer, int amount);

struct connection{
	void *data;

	TGetSinceConnection getEstablishedSince;
	TGetIdClientConnection getIdClient;
	TGetPeerConnection getClient;
	TGetStatusConnection getStatusClient;
	TGetAttemptsConnection getAttemptsClient;
	TSetIdClientConnection setIdClient;
	TSetStatusConnection setStatusClient;
	TSetAttemptsConnection setAttemptsClient;
	TGetIdServerConnection getIdServer;
	TGetPeerConnection getServer;
	TGetStatusConnection getStatusServer;
	TSetIdServerConnection setIdServer;
	TSetStatusConnection setStatusServer;
	TSetSinceConnection setSince;
	TGetIndexOfSatisfactionStatsConnection getIndexSatisfactionStats;
	TUpdateRequestsStatsConnection updateRequestsStats;
	TUpdateHitsStatsConnection updateHitsStats;

	TUFreeConnection ufree;

};

#endif /* CONNECTION_H_ */
