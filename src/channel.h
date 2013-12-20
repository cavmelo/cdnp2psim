typedef struct channel TChannel;

TChannel *createDataChannel(float capacity, float rate_upload, float percentDownlinkRatePercent);

typedef  short (* TCanStreamDataChannel )(TChannel *, float rate, int prefetch);
typedef  float (* TGetULRate )(TChannel *);
typedef  float (* TGetDLRate )(TChannel *);
typedef  short (* TOpenDLDataChannel )(TChannel *, int idPeerSrc, int idPeerDst, float rate, int prefetch);
typedef  short (* TOpenULDataChannel )(TChannel *, int idPeerSrc, int idPeerDst, float rate, int prefetch);
typedef  void (* TCloseDLDataChannel )(TChannel *, unsigned int idPeerDst);
typedef  void (* TCloseULDataChannel )(TChannel *, unsigned int idPeerDst);
typedef  short (* THasDownlinkChannel )(TChannel *, float bitRate, int prefetch);

struct channel{
	void *data;
	TGetULRate getULRate;
	TGetDLRate getDLRate;
	TCanStreamDataChannel canStream;
	TOpenDLDataChannel openDL; // open a DownLink data Channel
	TOpenULDataChannel openUL;
	TCloseDLDataChannel closeDL; // close a DownLink data Channel
	TCloseULDataChannel closeUL;
	THasDownlinkChannel hasDownlink;
};

