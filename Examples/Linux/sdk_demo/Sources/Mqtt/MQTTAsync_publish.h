#ifndef _MQTTASYNC_PUBLISH_H_
#define _MQTTASYNC_PUBLISH_H_

#include "MQTTAsync.h"

MQTTAsync initiateMQTTConnection(char* Address, char* ClientID);

//int publishMqttMsgOnTopic(MQTTAsync client, char* topic, unsigned char* data, int datalen);
int publishMqttMsgOnTopic(MQTTAsync client, char* topic, void* data, int datalen);

void disconnectMQTTConnection(MQTTAsync client);

#endif // _MQTTASYNC_PUBLISH_H_
