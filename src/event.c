#include "stdio.h"
#include "stdlib.h"
#include "event.h"

typedef struct _data_event TDataEvent;
struct _data_event{
    TTimeEvent time;
    TTypeEvent type;
    TOwnerEvent owner;
};

static TTimeEvent getTimeEvent(TEvent* event){
	TDataEvent *data = event->data;
	return data->time;
}

static TTypeEvent getTypeEvent(TEvent* event){
	TDataEvent *data = event->data;
	return data->type;
}

static TOwnerEvent getOwnerEvent(TEvent* event){
	TDataEvent *data = event->data;
	return data->owner;
}

static void ufreeEvent(TEvent* event){
	TDataEvent *data = event->data;

	free(data);
	free(event);
}

TEvent *createEvent(TTimeEvent time, TTypeEvent type, TOwnerEvent owner){
    TEvent *event;
    TDataEvent *data = malloc(sizeof(TDataEvent));

    event = (TEvent *)malloc(sizeof(TEvent));

    data->time = time;
    data->type = type;
    data->owner = owner;

    event->data = data;
    event->getOwner = getOwnerEvent;
    event->getTime = getTimeEvent;
    event->getType = getTypeEvent;
    event->ufree = ufreeEvent;

    return event;
}

