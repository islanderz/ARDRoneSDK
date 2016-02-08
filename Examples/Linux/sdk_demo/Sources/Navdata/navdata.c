#include <ardrone_tool/Navdata/ardrone_navdata_client.h>

#include <Navdata/navdata.h>
#include <stdio.h>
#include <Mqtt/MQTTAsync_publish.h>
#include <binn.h>
#include <time.h>

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
    
    //HS_02082016
    //these are the fields that are required in the ROS Ardrone_autonomy Navdata.msg which we want to publish on the ROS side.
    //reference has been takjen from ardrone_navdata_file.c

    binn_object_set_uint32(obj, "vbat_flying_percentage", nd->vbat_flying_percentage);
    binn_object_set_uint32(obj, "ctrl_state", nd->ctrl_state);
    //    //HS_02082016 - Apparently tum_ardrone doesn't require mx,my,mz
    //binn_object_set_int32(obj, "mx", navdata->navdata_magneto.mx);
    //binn_object_set_int32(obj, "my", navdata->navdata_magneto.my);
    //binn_object_set_int32(obj, "mz", navdata->navdata_magneto.mz);
    //(signed int) pnd->navdata_magneto.mx,
    //(signed int) pnd->navdata_magneto.my,
    //(signed int) pnd->navdata_magneto.mz

    binn_object_set_int32(obj, "pressure", navdata->navdata_pressure_raw.Pression_meas);

    //Doesn't seem to be required by tum_ardrone
    //(signed int) pnd->navdata_pressure_raw.Temperature_meas
    //pnd->navdata_wind_speed.wind_speed,
    //pnd->navdata_wind_speed.wind_angle,
    //pnd->navdata_wind_speed.wind_compensation_phi,

    binn_object_set_float(obj, "theta", nd->theta);
    binn_object_set_float(obj, "phi", nd->phi);
    binn_object_set_float(obj, "psi", nd->psi);
    binn_object_set_uint32(obj, "altitude", nd->altitude);
    binn_object_set_float(obj, "vx", nd->vx);
    binn_object_set_float(obj, "vy", nd->vy);
    binn_object_set_float(obj, "vz", nd->vz);
    //quick grep in tum_ardrone says we don't need this: HS02082016
    //navdata->navdata_phys_measures.phys_accs[ACC_X],
    //navdata->navdata_phys_measures.phys_accs[ACC_Y],
    //navdata->navdata_phys_measures.phys_accs[ACC_Z]

    binn_object_set_uint32(obj, "motor1", navdata->navdata_pwm.motor1);
    binn_object_set_uint32(obj, "motor2", navdata->navdata_pwm.motor2);
    binn_object_set_uint32(obj, "motor3", navdata->navdata_pwm.motor3);
    binn_object_set_uint32(obj, "motor4", navdata->navdata_pwm.motor4);
    //(unsigned int) pnd->navdata_vision_detect.nb_detected 
    //missing tags loop
    binn_object_set_uint32(obj, "tm", navdata->navdata_time.time);

    /* Apparently not required
    binn_object_set_uint32(obj, "timestamp", (uint32_t)time(NULL));
    binn_object_set_uint16(obj, "tag", nd->tag);
    binn_object_set_uint16(obj, "size", nd->size);
    binn_object_set_uint32(obj, "num_frames", nd->num_frames);
    binn_object_set_uint32(obj, "detection_camera_type", nd->detection_camera_type);
    */

    //publish data from the binn using mqtt
    publishMqttMsgOnTopic(client,"uas/uav1/navdata", binn_ptr(obj), binn_size(obj));
    
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

