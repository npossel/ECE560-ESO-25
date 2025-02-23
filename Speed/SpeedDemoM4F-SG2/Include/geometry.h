#ifndef GEOMETRY_H
#define GEOMETRY_H
// geometry 

#include "config.h"
void Find_Nearest_Waypoint(float cur_pos_lat, float cur_pos_lon, float * distance, float * bearing, 
	char  * * name);

typedef struct {
	float Lat;
	float SinLat;
	float CosLat;
	float Lon;
	char Name[24];
} GPT_T;
void Find_Nearest_Waypoint(float cur_pos_lat, float cur_pos_lon, float * distance, float * bearing, 
	char  * * name);

#endif
