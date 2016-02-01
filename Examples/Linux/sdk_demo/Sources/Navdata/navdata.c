#include <ardrone_tool/Navdata/ardrone_navdata_client.h>

#include <Navdata/navdata.h>
#include <stdio.h>
#include <Mqtt/MQTTAsync_publish.h>
#include <binn.h>

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
    //serializing using binn library.

    //initialize obj
    binn* obj;
    obj = binn_object();
    
    //all fields taken from navdata_demo_t structure in navdata_common.h
    
    //add values

    binn_object_set_uint16(obj, "tag", nd->tag);
    binn_object_set_uint16(obj, "size", nd->size);
    binn_object_set_uint32(obj, "ctrl_state", nd->ctrl_state);
    binn_object_set_uint32(obj, "vbat_flying_percentage", nd->vbat_flying_percentage);
    binn_object_set_float(obj, "theta", nd->theta);
    binn_object_set_float(obj, "phi", nd->phi);
    binn_object_set_float(obj, "psi", nd->psi);
    binn_object_set_uint32(obj, "altitude", nd->altitude);
    binn_object_set_float(obj, "vx", nd->vx);
    binn_object_set_float(obj, "vy", nd->vy);
    binn_object_set_float(obj, "vz", nd->vz);
    binn_object_set_uint32(obj, "num_frames", nd->num_frames);
    binn_object_set_uint32(obj, "detection_camera_type", nd->detection_camera_type);
    //binn_object_set_str(obj, "name", "John");

    //publish data from the binn using mqtt
    publishMqttMsgOnTopic(client,"uas/ardrone1/navdata", binn_ptr(obj), binn_size(obj));
    
    // release the buffer
    binn_free(obj);
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

