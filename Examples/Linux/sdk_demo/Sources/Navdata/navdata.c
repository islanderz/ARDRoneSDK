#include <ardrone_tool/Navdata/ardrone_navdata_client.h>

#include <Navdata/navdata.h>
#include <stdio.h>
#include <Mqtt/MQTTAsync_publish.h>

/* Initialization local variables before event loop  */
inline C_RESULT demo_navdata_client_init( void* data )
{
  return C_OK;
  //TODO SuREKA - for initializing connection just once, we can probably initialize a global 
  //variable MQTTAsync Client and then call the initiateMQTTConnection() function here!
  //Should test once we are sure this one works.
}

/* Receving navdata during the event loop */
inline C_RESULT demo_navdata_client_process( const navdata_unpacked_t* const navdata )
{
	const navdata_demo_t* nd = &navdata->navdata_demo;

  //d
	printf("=============================  Publishing data ===============================");

	//publishText();
  
  //Format: initiateMQTTConnection(Server, ClientID);

  //Test this by initiaing a persistent connection and not connecting each time.
  MQTTAsync client = initiateMQTTConnection("tcp://unmand.io:1884","ExampleClientPub");

  char data[15];
  sprintf(data, "%i", nd->altitude);
  const char topic[] = "navdata/altd";
  publishMqttMsgOnTopic(client,topic, data);

  disconnectMQTTConnection(client);


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
  return C_OK;
}

/* Registering to navdata client */
BEGIN_NAVDATA_HANDLER_TABLE
  NAVDATA_HANDLER_TABLE_ENTRY(demo_navdata_client_init, demo_navdata_client_process, demo_navdata_client_release, NULL)
END_NAVDATA_HANDLER_TABLE

