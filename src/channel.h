typedef struct channel TChannel;

TChannel *createDataChannel(float capacity, float rate_upload, float percentDownlinkRatePercent);

typedef  short (* TCanStreamDataChannel )(TChannel *, float rate);
typedef  float (* TGetULRate )(TChannel *);
typedef  float (* TGetDLRate )(TChannel *);
typedef  short (* TOpenDLDataChannel )(TChannel *, int idPeerSrc, int idPeerDst, float rate);
typedef  short (* TOpenULDataChannel )(TChannel *, int idPeerSrc, int idPeerDst, float rate);
typedef  void (* TCloseDLDataChannel )(TChannel *, unsigned int idPeerDst);
typedef  void (* TCloseULDataChannel )(TChannel *, unsigned int idPeerDst);
typedef  short (* THasDownlinkChannel )(TChannel *);

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

