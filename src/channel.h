typedef struct channel TChannel;

TChannel *createDataChannel(float capacity, float rate_upload);

typedef  short (* TCanStreamDataChannel )(TChannel *, float rate);
typedef  short (* TOpenDLDataChannel )(TChannel *, int idPeerSrc, int idPeerDst, float rate);
typedef  short (* TOpenULDataChannel )(TChannel *, int idPeerSrc, int idPeerDst, float rate);
typedef  void (* TCloseDLDataChannel )(TChannel *, unsigned int idPeerDst);
typedef  void (* TCloseULDataChannel )(TChannel *, unsigned int idPeerDst);
typedef  short (* THasDownlinkChannel )(TChannel *);

struct channel{
	void *data;
	TCanStreamDataChannel canStream;
	TOpenDLDataChannel openDL; // open a DownLink data Channel
	TOpenULDataChannel openUL;
	TCloseDLDataChannel closeDL; // close a DownLink data Channel
	TCloseULDataChannel closeUL;
	THasDownlinkChannel hasDownlink;
};

