#include <ardrone_tool/Navdata/ardrone_navdata_client.h>

#include <Navdata/navdata.h>
#include <stdio.h>
#include <Mqtt/MQTTAsync_publish.h>
MQTTAsync client;


/* Initialization local variables before event loop  */
inline C_RESULT demo_navdata_client_init( void* data )
{
  client = initiateMQTTConnection("tcp://unmand.io:1884","ArdroneSDkNavdataClient");
  return C_OK;
}

/* Receving navdata during the event loop */
inline C_RESULT demo_navdata_client_process( const navdata_unpacked_t* const navdata )
{
	const navdata_demo_t* nd = &navdata->navdata_demo;

  printf("=============================  Publishing data ===============================");

  if(client != NULL)
  {
    //TODO:Sureka: We are sending each value in a separate message. Optimize this using serialization.

    //altitude
    unsigned char altitudeBuffer[sizeof(int32_t)];
    int2Bytes(nd->altitude,&altitudeBuffer[0]);
    publishMqttMsgOnTopic(client,"navdata/altd", altitudeBuffer, sizeof(int32_t));

    //orientation
    //Add the floats to the orientationBuffer - one float at a time and then publish it.
    unsigned char orientationBuffer[3*sizeof(float)];
    int byteCounter = 0;
    float2Bytes(nd->theta,&orientationBuffer[byteCounter]);
    byteCounter += sizeof(float);
    float2Bytes(nd->phi,&orientationBuffer[byteCounter]);
    byteCounter += sizeof(float);
    float2Bytes(nd->psi,&orientationBuffer[byteCounter]);
    publishMqttMsgOnTopic(client,"navdata/orientation", orientationBuffer, 3*sizeof(float));
  }
  else
  {
    printf("Error in MQTT Connection..");
  }


	printf("=====================\nNavdata for flight demonstrations =====================\n\n");

	printf("Control state : %i\n",nd->ctrl_state);
	printf("Battery level : %i mV\n",nd->vbat_flying_percentage);
	printf("Orientation   : [Theta] %4.3f  [Phi] %4.3f  [Psi] %4.3f\n",nd->theta,nd->phi,nd->psi);
	printf("Altitude      : %i\n",nd->altitude);
	printf("Speed         : [vX] %4.3f  [vY] %4.3f  [vZPsi] %4.3f\n",nd->theta,nd->phi,nd->psi);

	printf("\033[8A");

  return C_OK;
}

/* Relinquish the local resources after the event loop exit */
inline C_RESULT demo_navdata_client_release( void )
{
  disconnectMQTTConnection(client);
  return C_OK;
}

/* Registering to navdata client */
BEGIN_NAVDATA_HANDLER_TABLE
  NAVDATA_HANDLER_TABLE_ENTRY(demo_navdata_client_init, demo_navdata_client_process, demo_navdata_client_release, NULL)
END_NAVDATA_HANDLER_TABLE

