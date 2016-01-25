#ifndef _NAVDATA_H_
#define _NAVDATA_H_

#include <ardrone_tool/Navdata/ardrone_navdata_client.h>

/** Helped Functions to convert integer or float into a byteArray*/
void float2Bytes(float val, unsigned char* bytes_array){
  union {
    float float_variable;
    unsigned char temp_array[sizeof(float)];
  } u;
  u.float_variable = val;
  memcpy(bytes_array, u.temp_array, sizeof(float));
}
void int2Bytes(int32_t val, unsigned char* bytes_array){
  union {
    int32_t float_variable;
    unsigned char temp_array[sizeof(int)];
  } u;
  u.float_variable = val;
  memcpy(bytes_array, u.temp_array, sizeof(int32_t));
}
/************/

#endif // _NAVDATA_H_
