#ifndef _MQTTASYNC_PUBLISH_H_
#define _MQTTASYNC_PUBLISH_H_

#include "MQTTAsync.h"

MQTTAsync initiateMQTTConnection(char* Address, char* ClientID);

int publishMqttMsgOnTopic(MQTTAsync client, char* topic, char* data);

void disconnectMQTTConnection(MQTTAsync client);

#endif // _MQTTASYNC_PUBLISH_H_
