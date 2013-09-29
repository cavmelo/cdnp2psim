/*
 * connection.c

 *
 *  Created on: Dec 22, 2012
 *      Author: cesar
 */
#include "stdio.h"
#include "stdlib.h"
#include "internals.h"
#include "system.h"
#include "dictionary.h"
#include "connection.h"


enum {UNDEFINED=0, REQUESTED=1, ACCEPTED=2, CONNECTED=3, TEARDOWN=4, CLOSED=5, REJECTED=6, ABORTED=7, CLEANED=8};

/* ! stats for a going on connection from a client side
 *
 */
struct _data_stats_client_connection{
	int requests;
	int hits;
};

/* ! stats for a going on connection from a client side
 *
 */
struct _data_stats_server_connection{
	int requests;
	int hits;
};

/*! \
/
*/
typedef struct _data_connection TDataConnection;
struct _data_client_connection{
	void *peer; //!< peer that has requested this connection
	unsigned int idClient; //!< id of peer that has requested  this connection
	short status; //!< connected, pending, tear down (disconnected)
	short attempts; //!< record how many times the connection has been attempted
	struct _data_stats_client_connection stats; //!< hold stats from this connection, e.g amount of transfered data
};

struct _data_server_connection{
	unsigned int idServer; /*!< id of peer that has been requested to accept a connection */
	void *peer; //!< peer that has been requested
	short status; //!< connected, pending, tear down (disconnected)
	struct _data_stats_server_connection stats; //!< stats about this connection from server point.
};

/*!
 * \brief describes a data connection among two peers.
 * hold information about two side of a connection and info about the connections itself (how long it has been UP).
 */
struct _data_connection{
	struct _data_server_connection server;
	struct _data_client_connection client;
	unsigned long int since; // ! connection established since (time)
};


/*!
 * initialize data connection with passed ids and peer's addresses.
 * A couple of connection info is kept default, e.g connection status.
 */
TDataConnection *initDataConnection(unsigned int idClient, void *client, unsigned int idServer, void *server){
	TDataConnection *data = malloc(sizeof(TDataConnection));


	data->client.idClient = idClient;
	data->client.peer = client;
	data->client.status = UNDEFINED;
	data->client.attempts = 0;
	data->client.stats.hits = 0;
	data->client.stats.requests = 0;

	data->server.idServer = idServer;
	data->server.peer = server;
	data->server.status = UNDEFINED;
	data->server.stats.hits = 0;
	data->server.stats.requests = 0;

	data->since = 0;
	return data;
}


static unsigned long int getEstablishedSinceConnection(TConnection *conn){
	TDataConnection *d = conn->data;

	return d->since;
}
/*
 *\brief returns the id of peer which started a connection
 */
static unsigned int getIdClientConnection(TConnection *conn){
	TDataConnection *d = conn->data;

	return d->client.idClient;
}

static unsigned int getIdServerConnection(TConnection *conn){
	TDataConnection *d = conn->data;

	return d->server.idServer;
}

static short getStatusServerConnection(TConnection *conn){
	TDataConnection *d = conn->data;

	return d->server.status;
}

static void* getPeerServerConnection(TConnection *conn){
	TDataConnection *d = conn->data;

	return d->server.peer;
}

static short getStatusClientConnection(TConnection *conn){
	TDataConnection *d = conn->data;

	return d->client.status;
}

static short getAttemptsClientConnection(TConnection *conn){
	TDataConnection *d = conn->data;

	return d->client.attempts;
}

static void* getPeerClientConnection(TConnection *conn){
	TDataConnection *d = conn->data;

	return d->client.peer;
}

static void setIdClientConnection(TConnection *conn, unsigned int idClient){
	TDataConnection *d = conn->data;

	d->client.idClient = idClient;
}

static void setIdServerConnection(TConnection *conn, unsigned int idServer){
	TDataConnection *d = conn->data;

	d->server.idServer = idServer;
}

static void setStatusClientConnection(TConnection *conn, short status){
	TDataConnection *d = conn->data;

	d->client.status = status;
}

static void setStatusServerConnection(TConnection *conn, short status){
	TDataConnection *d = conn->data;

	d->server.status = status;
}

static void setAttemptsClientConnection(TConnection *conn, short attempts){
	TDataConnection *d = conn->data;

	d->client.attempts = attempts;
}

static void setSinceConnection(TConnection *conn, unsigned long int since){
	TDataConnection *d = conn->data;

	d->since = since;
}

static float getIndexOfStatisfactionConnectionStats( TConnection *conn, int idPeer ){
	TDataConnection *d = conn->data;
	if (d->client.idClient == idPeer){
		return ((d->client.stats.requests?(float)d->client.stats.hits/d->client.stats.requests:0.0));
	}else{
		return ((d->server.stats.requests?(float)d->server.stats.hits/d->server.stats.requests:0.0));

	}
}

static void updateHitsStatsConnection(TConnection *conn, int idPeer, int amount){
	TDataConnection *d = conn->data;
	if (d->client.idClient == idPeer){
		d->client.stats.hits += amount;
	}else{
		d->server.stats.hits += amount;
	}
}

static void updateRequestsStatsConnection(TConnection *conn, int idPeer, int amount){
	TDataConnection *d = conn->data;
	if (d->client.idClient == idPeer){
		d->client.stats.requests += amount;
	}else{
		d->server.stats.requests += amount;
	}
}


static void freeConnection(TConnection* conn){
	TDataConnection *data = conn->data;


	free(data);
	free(conn);
}

TConnection *createConnection(unsigned int idClient, void *client, unsigned int idServer, void*server){

	TConnection *conn = malloc(sizeof(TConnection));

	conn->data = initDataConnection(idClient, client, idServer,server);

	conn->getEstablishedSince = getEstablishedSinceConnection;
	conn->getIdClient = getIdClientConnection;
	conn->getClient = getPeerClientConnection;
	conn->getServer = getPeerServerConnection;
	conn->getIdServer = getIdServerConnection;
	conn->getStatusClient = getStatusClientConnection;
	conn->getStatusServer = getStatusServerConnection;
	conn->getAttemptsClient = getAttemptsClientConnection;
	conn->setIdClient = setIdClientConnection;
	conn->setIdServer = setIdServerConnection;
	conn->setStatusClient = setStatusClientConnection;
	conn->setStatusServer = setStatusServerConnection;
	conn->setAttemptsClient = setAttemptsClientConnection;
	conn->setSince = setSinceConnection;

	conn->getIndexSatisfactionStats = getIndexOfStatisfactionConnectionStats;
	conn->updateHitsStats = updateHitsStatsConnection;
	conn->updateRequestsStats = updateRequestsStatsConnection;
	conn->ufree = freeConnection;

	return conn;
}


typedef struct _data_connBoard TDataConnBoard;
typedef struct connBoard TConnBoard;

typedef void (*TInsertConnBoard)(TConnBoard*, void *conn);
typedef void (*TRemoveConnBoard)(TConnBoard*, void *conn);
typedef void* (*TRetrievalConnBoard)(TConnBoard*, unsigned int idPeer);
typedef short (*THasConnectionConnBoard)(TConnBoard*, unsigned int idPeer);

struct _data_connBoard{
	TMultiDictionary *connClient;
	TMultiDictionary *connServer;
};

struct connBoard{
	void *data;
	TInsertConnBoard insert;
	TRemoveConnBoard removeAsClient;
	TRemoveConnBoard removeAsServer;
	TRetrievalConnBoard retrievalAsClient;
	TRetrievalConnBoard retrievalAsServer;
	THasConnectionConnBoard hasConnectionAsClient;
	THasConnectionConnBoard hasConnectionAsServer;
	THasConnectionConnBoard hasConnection;
};

static short criteriaCompareConnBoard(void* xstored, void* xconn){
	TConnection *conn = xconn;
	TDataConnection *dataConn = conn->data;
	TConnection *stored = xstored;
	TDataConnection *dataStored = stored->data;

	if ( (dataConn->server.idServer == dataStored->server.idServer) && (dataConn->client.idClient==dataStored->client.idClient))
		return 0;
	else
		return 1;

}


static TDataConnBoard *initDataConnBoard(){
	TDataConnBoard *data = malloc(sizeof(TDataConnBoard));

	data->connClient = createMultiDictionary(criteriaCompareConnBoard);
	data->connServer = createMultiDictionary(criteriaCompareConnBoard);

	return data;
}


static void insertConnBoard(TConnBoard *connBoard, void *xconn){
	TConnection *conn = xconn;
	TDataConnection *dataConn = conn->data;

	TDataConnBoard *dataBoard = connBoard->data;
	TMultiDictionary *connClient = dataBoard->connClient;
	TMultiDictionary *connServer = dataBoard->connServer;

	//char xIdClient[20];sprintf(xIdClient,"%u",dataConn->idClient);
	TKeyDictionary keyIdClient = dataConn->client.idClient; //connClient->keyGenesis(xIdClient);
	connClient->insert(connClient,keyIdClient,conn);


	//char xIdServer[20];sprintf(xIdServer,"%u",dataConn->idServer);
	TKeyDictionary keyIdServer = dataConn->server.idServer; //connServer->keyGenesis(xIdServer);

	connServer->insert(connServer,keyIdServer,conn);


}

static void removeServerConnBoard(TConnBoard *connBoard, void *xconn){
	TConnection *conn = xconn;
	TDataConnection *dataConn = conn->data;
	TDataConnBoard *dataBoard = connBoard->data;
	TMultiDictionary *connServer = dataBoard->connServer;


	//char xIdServer[20];sprintf(xIdServer,"%u",dataConn->idServer);
	TKeyDictionary keyIdServer = dataConn->server.idServer; //connServer->keyGenesis(xIdServer);
	connServer->remove(connServer, keyIdServer, conn);


}

static void removeClientConnBoard(TConnBoard *connBoard, void *xconn){
	TConnection *conn = xconn;
	TDataConnection *dataConn = conn->data;
	TDataConnBoard *dataBoard = connBoard->data;
	TMultiDictionary *connClient = dataBoard->connClient;

	//char xIdClient[20];sprintf(xIdClient,"%d",dataConn->idClient);
	TKeyDictionary keyIdClient = dataConn->client.idClient; //connClient->keyGenesis(xIdClient);
	connClient->remove(connClient,keyIdClient,conn);

}

//return a Iterator of a List of Connections Requests;
static void* retrievalConnBoard(TConnBoard *connBoard, unsigned int idPeer, TMultiDictionary *conn){

	//char xIdPeer[20];sprintf(xIdPeer,"%u",idPeer);
	TKeyDictionary key = idPeer; // conn->keyGenesis(xIdPeer);

	TListGeneral *l = conn->retrieval(conn, key);

	TIterator *it = NULL;
	if (l)
		it = createIteratorListGeneral(l);

	return it;
};


//return a Iterator of a List of Connections Requests;
static void* retrievalClientConnBoard(TConnBoard *connBoard, unsigned int idPeer){
	TDataConnBoard *data = connBoard->data;
	TMultiDictionary *connClient = data->connClient;


	TIterator *it = retrievalConnBoard(connBoard, idPeer, connClient);

	return it;
};

//return a Iterator of a List of Connections Requests;
static void* retrievalServerConnBoard(TConnBoard *connBoard, unsigned int idPeer){
	TDataConnBoard *data = connBoard->data;
	TMultiDictionary *connServer = data->connServer;

	TIterator *it = retrievalConnBoard(connBoard, idPeer, connServer);

	return it;
};


//return a True or False for any Connection Request;
static short hasConnectionConnBoard(TConnBoard *connBoard, unsigned int idPeer){
	TDataConnBoard *data = connBoard->data;
	TMultiDictionary *connAsClient = data->connClient;
	TMultiDictionary *connAsServer = data->connServer;
	TKeyDictionary key = idPeer;

	TListGeneral *l = connAsClient->retrieval(connAsClient, key);
	if (!l)
		l = connAsServer->retrieval(connAsServer, key);
	return (l?1:0);
}


static short hasConnectionAsClientConnBoard(TConnBoard *connBoard, unsigned int idPeer){
	TDataConnBoard *data = connBoard->data;
	TMultiDictionary *connAsClient = data->connClient;

	TKeyDictionary key = idPeer;

	TListGeneral *l = connAsClient->retrieval(connAsClient, key);

	return (l?1:0);

}


static short hasConnectionAsServerConnBoard(TConnBoard *connBoard, unsigned int idPeer){
	TDataConnBoard *data = connBoard->data;
	TMultiDictionary *connAsServer = data->connServer;

	TKeyDictionary key = idPeer;

	TListGeneral *l = connAsServer->retrieval(connAsServer, key);

	return (l?1:0);
}


TConnBoard *createConnBoard(){
	static TConnBoard *connBoard = NULL;

	if (connBoard)
		return connBoard;

	connBoard = malloc(sizeof(TConnBoard));

	connBoard->data = initDataConnBoard();
	connBoard->insert = insertConnBoard;
	connBoard->removeAsClient = removeClientConnBoard;
	connBoard->removeAsServer = removeServerConnBoard;
	connBoard->retrievalAsClient = retrievalClientConnBoard;
	connBoard->retrievalAsServer = retrievalServerConnBoard;
	connBoard->hasConnection = hasConnectionConnBoard;
	connBoard->hasConnectionAsClient = hasConnectionAsClientConnBoard;
	connBoard->hasConnectionAsServer = hasConnectionAsServerConnBoard;

	return connBoard;
}


typedef struct _data_conn_manager TDataConnManager;
struct _data_conn_manager{
	TArrayDynamic *connectedAsServer; // all running connections as Server
	TArrayDynamic *connectedAsClient; // all running connections as Client
	TArrayDynamic *connected; // all running connections
	TConnBoard *connBoard;
	unsigned int idOwner; // id peer which owns this connection manager
	void *owner;
	int maxConnections;
	int maxAttempts;
	int numEstablishedConnections;
	int numPendingConnections;
};



static TDataConnManager* initDataConnManager(int maxConnections, int maxAttempts, unsigned int idOwner, void *owner){

	TDataConnManager *data = malloc(sizeof(TDataConnManager));
	TArrayDynamic *connectedAsServer = createArrayDynamic(0);
	TArrayDynamic *connectedAsClient = createArrayDynamic(0);
	TArrayDynamic *connected = createArrayDynamic(0);

	data->connBoard = createConnBoard();
	data->connectedAsServer =  connectedAsServer;
	data->connectedAsClient =  connectedAsClient;
	data->connected = connected;
	data->maxConnections = maxConnections;
	data->maxAttempts = maxAttempts;
	data->idOwner = idOwner;
	data->owner = owner;
	data->numEstablishedConnections = 0;
	data->numPendingConnections = 0;

	return data;
}

static short hasEstablishedConnectionConnManager(TConnManager *connManager, unsigned int idPeer){
	TDataConnManager *data = connManager->data;

	TConnection *conn = data->connectedAsClient->retrieval(data->connectedAsClient,idPeer);
	if (!conn)
		conn = data->connectedAsServer->retrieval(data->connectedAsServer,idPeer);

	return (conn?1:0);
}


static short openConnectionConnManager(TConnManager *connManager, unsigned int idServer, void *server){
	short status=0;
	TDataConnManager *data = connManager->data;
	TConnBoard *board = data->connBoard;
	TArrayDynamic *connections = data->connectedAsClient;

	if (data->maxConnections > (data->numEstablishedConnections+data->numPendingConnections)){
		if ( !(connManager->hasEstablishedConnection(connManager,idServer)) ){
			TConnection *conn = createConnection(data->idOwner, data->owner, idServer, server);

			conn->setStatusClient(conn,REQUESTED);      // connection status

			connections->insert(connections, idServer, conn);
			board->insert(board,conn);

			data->numPendingConnections++;
			status=1;
		}
	}

	return status;
}

//enum {UNDEFINED=0, REQUESTED=1, ACCEPTED=2, CONNECTED=3, TEARDOWN=4, CLOSED=5, REJECTED=6, ABORTED=7, CLEANED=8};

static void EvaluateClientConnection(TConnManager *connManager, void *vSysInfo){
	TSystemInfo *sysInfo = vSysInfo;
	TDataConnManager *data = connManager->data;
	TConnBoard *board = data->connBoard;
	TArrayDynamic *connections = data->connectedAsClient;
	TConnection *conn;
	unsigned int idPeer = data->idOwner;

	TIterator *it = board->retrievalAsClient(board,idPeer);
	if (it==NULL)
		return;

	it->reset(it);
	while(it->has(it)){
		conn = it->current(it);
		it->next(it);

		short statusServer = conn->getStatusServer(conn);
		short statusClient = conn->getStatusClient(conn);
		//printf("Client: %p\n", conn); fflush(stdout);
		//printf("from: S: %d %d  C: %d %d\n", conn->getStatusServer(conn),conn->getIdServer(conn), conn->getStatusClient(conn), conn->getIdClient(conn) );

		if ( (statusClient == REQUESTED) && (statusServer == ACCEPTED)){
			conn->setStatusClient(conn, CONNECTED);
			conn->setStatusServer(conn, CONNECTED);
			conn->setSince(conn, sysInfo->getTime(sysInfo));
			data->numEstablishedConnections++;
			data->numPendingConnections--;

		}else if( (statusClient == REQUESTED) && (statusServer == UNDEFINED) ){
			int attempts = conn->getAttemptsClient(conn) + 1;
			conn->setAttemptsClient(conn,attempts);
			if (attempts > data->maxAttempts){
				conn->setStatusClient(conn, ABORTED);
			}

		}else if((statusServer == CLEANED)){
			board->removeAsClient(board,conn);
			connections->remove(connections,conn->getIdServer(conn));
			data->numPendingConnections--;

			conn->ufree(conn);

		}else if ( (statusServer == TEARDOWN) && (statusClient == TEARDOWN)) {
			board->removeAsClient(board,conn);
			connections->remove(connections,conn->getIdServer(conn));
			data->numEstablishedConnections--;

			conn->ufree(conn);

		}else if ( (statusServer == TEARDOWN) && (statusClient == CONNECTED)) {
			board->removeAsClient(board,conn);
			connections->remove(connections,conn->getIdServer(conn));
			data->numEstablishedConnections--;

			conn->ufree(conn);

		}else if (statusServer == CLOSED) {
			board->removeAsClient(board,conn);
			connections->remove(connections,conn->getIdServer(conn));
			data->numEstablishedConnections--;

			conn->ufree(conn);

		}else if (statusServer == REJECTED){
			board->removeAsClient(board,conn);
			connections->remove(connections,conn->getIdServer(conn));
			data->numPendingConnections--;

			conn->ufree(conn);

		}

	}

	it->ufree(it);
}


static void evaluateServerConnection(TConnManager *connManager, TSystemInfo *sysInfo){
	TDataConnManager *data = connManager->data;
	TConnBoard *board = data->connBoard;
	TArrayDynamic *connections = data->connectedAsServer;
	TConnection *conn;

	unsigned int idPeer = data->idOwner;

	TIterator *it = board->retrievalAsServer(board,idPeer);
	if (it==NULL)
		return;

	it->reset(it);
	while(it->has(it)){
		conn = it->current(it);
		it->next(it);
		//printf("SERVER: %p\n", conn); fflush(stdout);
		short statusClient = conn->getStatusClient(conn);
		short statusServer = conn->getStatusServer(conn);
		//printf("TO: S: %d %d  C: %d %d\n", conn->getStatusServer(conn),conn->getIdServer(conn), conn->getStatusClient(conn), conn->getIdClient(conn) );

		if ((statusServer == UNDEFINED) && (statusClient == REQUESTED) ){
			if ( data->numEstablishedConnections+1 <= data->maxConnections ){
				data->numEstablishedConnections++;
				conn->setStatusServer(conn,ACCEPTED);
				connections->insert(connections,conn->getIdClient(conn),conn);
			}else{
				conn->setStatusServer(conn,REJECTED);
				board->removeAsServer(board,conn);
			}
		}else if((statusServer == UNDEFINED) && (statusClient == ABORTED)){
			conn->setStatusServer(conn,CLEANED);
			board->removeAsServer(board,conn);

		}else if( (statusServer == ACCEPTED) && (statusClient == TEARDOWN)) {
			conn->setStatusServer(conn,CLOSED);
			board->removeAsServer(board, conn);
			connections->remove(connections,conn->getIdClient(conn));

		}else if( (statusServer == CONNECTED) && (statusClient == TEARDOWN)) {
			data->numEstablishedConnections--;
			conn->setStatusServer(conn,CLOSED);
			board->removeAsServer(board, conn);
			connections->remove(connections,conn->getIdClient(conn));
		}

	}
	//printf("evaluateTO\n");
	it->ufree(it);

}

static void evalConnectionConnManager(TConnManager *connManager, void *sysInfo){

	EvaluateClientConnection(connManager, sysInfo);
	evaluateServerConnection(connManager, sysInfo);

}

static void closeServerConnection(TConnManager *connManager, unsigned int idClient, TConnection *conn){

	TDataConnManager *data = connManager->data;
	TArrayDynamic *connectionsServer = data->connectedAsServer;     // Connections started by other peers
	TConnBoard *board = data->connBoard;

	// update the other connection
	short statusServer = conn->getStatusServer(conn);
	short statusClient = conn->getStatusClient(conn);

	if (statusServer == CONNECTED){
		data->numEstablishedConnections--;
		conn->setStatusServer(conn, TEARDOWN);
		board->removeAsServer(board,conn);
		connectionsServer->remove(connectionsServer, idClient);

	}else if( (statusClient == REQUESTED) && (statusServer == ACCEPTED) ){
		data->numEstablishedConnections--;
		conn->setStatusServer(conn, CLEANED);
		board->removeAsServer(board,conn);
		connectionsServer->remove(connectionsServer, idClient);

	}else if( (statusClient == REQUESTED) && (statusServer == UNDEFINED)){
		conn->setStatusServer(conn, CLEANED);
		board->removeAsServer(board,conn);
	}

}


static void closeClientConnection(TConnManager *connManager, unsigned int idServer, TConnection *conn){
	TDataConnManager *data = connManager->data;
	// update the other connection
	short statusClient = conn->getStatusClient(conn);
	short statusServer = conn->getStatusServer(conn);

	if (statusClient == CONNECTED){
		conn->setStatusClient(conn, TEARDOWN);

	}else if(statusServer == ACCEPTED){
		conn->setStatusClient(conn, TEARDOWN);
		//data->connected++; // have to be reviewed

	}else if(statusClient == REQUESTED){
		conn->setStatusClient(conn, ABORTED);
		data->numPendingConnections--;
	}

}

static void closeConnectionConnManager(TConnManager *connManager, unsigned int idPeer){
	TDataConnManager *data = connManager->data;
	TArrayDynamic *connAsClient = data->connectedAsClient;     // Connecting as a client
	TConnBoard *board = data->connBoard;

	TConnection *conn = connAsClient->retrieval(connAsClient, idPeer);
	if (conn){
		closeClientConnection(connManager, idPeer, conn);
	}else{
		TIterator *it = board->retrievalAsServer(board, data->idOwner);
		if (it){
			it->reset(it);
			while(it->has(it)){
				conn = it->current(it);
				it->next(it);
				if (conn->getIdClient(conn) == idPeer){
					closeServerConnection(connManager, idPeer, conn);
				}
			}
			it->ufree(it);
		}
	}

}

static void teardownConnectionConnManager(TConnManager *connManager){
	TConnection *conn;
	TDataConnManager *data = connManager->data;
	TConnBoard *board = data->connBoard;

	TIterator *it = board->retrievalAsServer(board, data->idOwner);
	if (it){
		it->reset(it);
		while(it->has(it)){
			conn = it->current(it);
			it->next(it);
			closeServerConnection(connManager, conn->getIdClient(conn), conn);
		}
		it->ufree(it);
	}

	it = board->retrievalAsClient(board, data->idOwner);
	if (it){
		it->reset(it);
		while(it->has(it)){
			conn = it->current(it);
			it->next(it);
			closeClientConnection(connManager, conn->getIdServer(conn), conn);
		}
		it->ufree(it);
	}

}


static void showServerOngoingConnectionsConnManager(TConnManager *connManager){
	TConnection *conn;
	TDataConnManager *data = connManager->data;
	TArrayDynamic *connections = data->connectedAsServer;     // Connections started by other peers
	int i, occup = connections->getOccupancy(connections);
	for(i=0;i<occup;i++){
		conn = connections->getElement(connections,i);
		printf("s:%d %d %d %d\n",conn->getIdClient(conn),conn->getIdServer(conn), conn->getStatusClient(conn), conn->getStatusServer(conn));
	}
}


static void showClientOngoingConnectionsConnManager(TConnManager *connManager){
	TConnection *conn;
	TDataConnManager *data = connManager->data;
	TArrayDynamic *connections = data->connectedAsClient;     // Connections started by other peers
	int i, occup = connections->getOccupancy(connections);
	for(i=0;i<occup;i++){
		conn = connections->getElement(connections,i);
		printf("c:%d %d %d %d\n", conn->getIdClient(conn),conn->getIdServer(conn), conn->getStatusClient(conn), conn->getStatusServer(conn));
	}

}

static void showOngoingConnectionsConnManager(TConnManager *connManager){

	showClientOngoingConnectionsConnManager(connManager);
	//showServerConnection(connManager);

}


static void showClientEstablishedConnectionsConnManager(TConnManager *connManager){
	TConnection *conn;
	TDataConnManager *data = connManager->data;
	TArrayDynamic *connections = data->connectedAsClient;     // Connections started by other peers
	int i, occup = connections->getOccupancy(connections);
	for(i=0;i<occup;i++){
		conn = connections->getElement(connections,i);
		if (conn->getStatusClient(conn) == CONNECTED)
			printf("c:%d %d %d %d\n", conn->getIdClient(conn),conn->getIdServer(conn), conn->getStatusClient(conn), conn->getStatusServer(conn));
	}

}


static void showEstablishedConnectionsConnManager(TConnManager *connManager){

	showClientEstablishedConnectionsConnManager(connManager);
	//showServerConnection(connManager);

}


static short hasPendingConnManager(TConnManager *connManager){
	TDataConnManager *data = connManager->data;

	return (data->numPendingConnections>0?1:0);
}

static short isConnectedConnManager(TConnManager *connManager){
	TDataConnManager *data = connManager->data;

	return (data->numEstablishedConnections>0?1:0);
}

static unsigned int getIdOwnerConnManager(TConnManager *connManager){
	TDataConnManager *data = connManager->data;

	return data->idOwner;
}

static void* getOwnerConnManager(TConnManager *connManager){
	TDataConnManager *data = connManager->data;

	return data->owner;
}

static void setOwnerConnManager(TConnManager *connManager, void* owner){
	TDataConnManager *data = connManager->data;

	data->owner = owner;
}

static int getNumEstablishedConnectionsConnManager(TConnManager *connManager){
	TDataConnManager *data = connManager->data;

	return data->numEstablishedConnections;
}

static short isFullConnectedConnManager(TConnManager *connManager){
	TDataConnManager *data = connManager->data;

//	return ((data->numEstablishedConnections+data->numPendingConnections)>=data->maxConnections);
	return (data->numEstablishedConnections >= data->maxConnections);
}

// return a dictionary with a list of connected peers
static void *getListEstablishedConnectionsConnManager(TConnManager *connManager){
	int i;
	TConnection *conn;
	TDataConnManager *data = connManager->data;
	TArrayDynamic *asClient = data->connectedAsClient;
	TArrayDynamic *asServer = data->connectedAsServer;
	TArrayDynamic *connected = data->connected;

	int ocuppAsClient = asClient->getOccupancy(asClient);
	int ocuppAsServer = asServer->getOccupancy(asServer);

	connected->removeAll(connected);

	for(i=0;i<ocuppAsClient;i++){
		conn = asClient->getElement(asClient,i);
		if ( (conn->getStatusClient(conn) == CONNECTED) && (conn->getStatusServer(conn) == CONNECTED))
			connected->insert(connected, conn->getIdServer(conn), conn);
	}

	for(i=0;i<ocuppAsServer;i++){
		conn = asServer->getElement(asServer,i);
		if ( (conn->getStatusClient(conn) == CONNECTED) && (conn->getStatusServer(conn) == CONNECTED))
			connected->insert(connected, conn->getIdClient(conn), conn);
	}

	return connected;
}


void updateRequestsStatsConnManager(TConnManager *connManager, unsigned int idPeer, int amount){
	int i;
	TConnection *conn;
	TDataConnManager *data = connManager->data;
	TArrayDynamic *asClient = data->connectedAsClient;
	TArrayDynamic *asServer = data->connectedAsServer;

	int ocuppAsClient = asClient->getOccupancy(asClient);
	int ocuppAsServer = asServer->getOccupancy(asServer);
	short done=0;
	for(i=0;(i<ocuppAsClient)&&(!done);i++){
		conn = asClient->getElement(asClient,i);
		if ( (conn->getIdClient(conn) == idPeer) || (conn->getIdServer(conn) == idPeer)){
			conn->updateRequestsStats(conn,idPeer,amount);
			done=1;
		}
	}

	for(i=0;(i<ocuppAsServer)&&(!done);i++){
		conn = asServer->getElement(asServer,i);
		if ( (conn->getIdClient(conn) == idPeer) || (conn->getIdServer(conn) == idPeer)){
			conn->updateRequestsStats(conn,idPeer,amount);
			done=1;
		}
	}
}


void updateHitsStatsConnManager(TConnManager *connManager, unsigned int idPeer, int amount){
	int i;
	TConnection *conn;
	TDataConnManager *data = connManager->data;
	TArrayDynamic *asClient = data->connectedAsClient;
	TArrayDynamic *asServer = data->connectedAsServer;

	int ocuppAsClient = asClient->getOccupancy(asClient);
	int ocuppAsServer = asServer->getOccupancy(asServer);
	short done=0;
	for(i=0;(i<ocuppAsClient)&&(!done);i++){
		conn = asClient->getElement(asClient,i);
		if ( (conn->getIdClient(conn) == idPeer) || (conn->getIdServer(conn) == idPeer)){
			conn->updateHitsStats(conn,idPeer,amount);
			done=1;
		}
	}

	for(i=0;(i<ocuppAsServer)&&(!done);i++){
		conn = asServer->getElement(asServer,i);
		if ( (conn->getIdClient(conn) == idPeer) || (conn->getIdServer(conn) == idPeer)){
			conn->updateHitsStats(conn,idPeer,amount);
			done=1;
		}
	}
}

static short hasOnGoingConnectionsConnManager(TConnManager *connManager){
	TDataConnManager *data = connManager->data;
	TConnBoard *board = data->connBoard;

	return board->hasConnection(board, data->idOwner);
}

static void disposeConnManager(TConnManager *connManager){
	TDataConnManager *data = connManager->data;

	TArrayDynamic *asClient = data->connectedAsClient;
	TArrayDynamic *asServer = data->connectedAsServer;

	asClient->ufree(asClient);
	asServer->ufree(asServer);

	free(data);
	free(connManager);
}

TConnManager * createConnManager(int maxConnections, int maxAttempts, unsigned int idPeer, void *peer){
	TConnManager* connManager = malloc(sizeof(TConnManager));
	connManager->data = initDataConnManager(maxConnections, maxAttempts, idPeer, peer);

	connManager->evaluateConnections = evalConnectionConnManager;
	connManager->teardownConnection = teardownConnectionConnManager;
	connManager->openConnection = openConnectionConnManager;
	connManager->hasEstablishedConnection = hasEstablishedConnectionConnManager;
	connManager->closeConnection = closeConnectionConnManager;
	connManager->hasPending = hasPendingConnManager;
	connManager->getIdOwner = getIdOwnerConnManager;
	connManager->getOwner = getOwnerConnManager;
	connManager->setOwner = setOwnerConnManager;
	connManager->isConnected = isConnectedConnManager;
	connManager->getNumEstablishedConnections = getNumEstablishedConnectionsConnManager;
	connManager->isFullConnected = isFullConnectedConnManager;
	connManager->getListEstablishedConnections = getListEstablishedConnectionsConnManager;
	connManager->hasOnGoingConnections = hasOnGoingConnectionsConnManager;
	connManager->showEstablishedConnections = showEstablishedConnectionsConnManager;
	connManager->showOngoingConnections = showOngoingConnectionsConnManager;

	connManager->updateHitsStats = updateHitsStatsConnManager;
	connManager->updateRequestsStats = updateRequestsStatsConnManager;

	//connManager->closeServerConnection = closeServerConnection;

	return connManager;
}


//#include "randomic.h"
//#define MAX 10
//struct _peer{
//	int id;
//	TConnManager *cm;
//};
//
//int main(){
//	struct _peer p[MAX];
//	TConnManager *cm;
//	unsigned int i, tm=0, op;
//
//	for (i=0;i<MAX;i++){
//		p[i].id = i;
//		p[i].cm = createConnManager(2,10,i);
//	}
//	int status=0;
//	char entry[30];sprintf(entry,"%d;",MAX);
//	TRandomic *rp = createUniformRandomic(entry);
//	TRandomic *ro = createUniformRandomic("3;");
//	do{
//
//		// sorteia pares
//		unsigned int p1 = rp->pick(rp)-1;
//		unsigned int p2 = rp->pick(rp)-1;
//
//		op = ro->pick(ro);
//
//		//scanf("%d%d%d", &op, &p1, &p2 );
//		//printf("Operacao: %d Pares P1: %d P2: %d\n", op, p1,p2 );
//		if (p1==p2){
//		}else if (op == 1){ // conexao
//			cm = p[p1].cm;
//			cm->openConnection(cm,p2);
//			status++;
//		}else if ( (op == -1) && (status>300) ){ // desconexao a partir do cliente
//			status=0;
//			cm = p[p1].cm;
//			cm->closeConnection(cm,p2);
//			//cm = p[p2].cm;
//			//cm->evaluateConnection(cm);
//		}else if( (op == -1) && (status>300)){ // desconexao a partir do servidor
//			cm = p[p2].cm;
//			cm->closeConnection(cm,p1);
//			status=0;
//			//cm = p[p1].cm;
//			//cm->evaluateConnection(cm);
//		}else{
//			cm = p[p1].cm;
//			cm->evaluateConnection(cm);
//			cm = p[p2].cm;
//			cm->evaluateConnection(cm);
//		}
//
//		// avança o tempo (tm)
//
//		tm++;
//	}while(tm<100000);
//
//	for(i=0;i<MAX;i++){
//
//		cm = p[i].cm;
//
//		//cm->showConnection(cm, i);
//		showConnection(cm,i);
//	}
//
//	return 0;
//}
