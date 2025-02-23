#include "geometry.h"
#include <math.h>
#include <string.h>
#define PI 3.14159265f
#define PI_OVER_180 (0.017453293f) // (3.1415927/180.0)

extern const GPT_T waypoints[];

/* 	VER
		11	Use radians in table, precalculate sin and cos for points
		12	Don't do acos or * 6371 until needed
		13	Don't do bearing until needed

		NOTE: NOT using merged CD/CB functions here
*/

#define VER 13  // Raise profiler sampling frequency for these versions

// VER 11
// Table holds precalculated sin/cos for p2. Table Lat/Lon values are in radians
float Calc_Distance( GPT_T * p1,  const GPT_T * p2) { 
// calculates distance in kilometers between locations (represented in radians)

  return acosf(
					p1->SinLat * p2->SinLat + 
					p1->CosLat * p2->CosLat * cosf(p2->Lon - p1->Lon)) * 6371;
}

float Calc_Bearing( GPT_T * p1,  const GPT_T * p2){
// calculates bearing in degrees between locations (represented in radians)	
	float term1, term2;
	float angle;
	
	term1 = sinf(p2->Lon - p1->Lon)*p2->CosLat;
	term2 = p1->CosLat*p2->SinLat - 
					p1->SinLat*p2->CosLat*cosf(p2->Lon - p1->Lon);
	angle = atan2f(term1, term2) * (180/PI);
	if (angle < 0) 
		angle += 360;
	return angle;
}

float Calc_Proximity( GPT_T * p1,  const GPT_T * p2) { 
// calculates closeness (decreases as distance increases) of locations
  return p1->SinLat * p2->SinLat + 
					p1->CosLat * p2->CosLat*
					cosf(p2->Lon - p1->Lon);
}

void Find_Nearest_Waypoint(float cur_pos_lat, float cur_pos_lon, float * distance, float * bearing, 
	char  * * name) {
	// cur_pos_lat and cur_pos_lon are in degrees
	// distance is in kilometers
	// bearing is in degrees
		
	int i=0, closest_i;
	GPT_T ref;
	float d, b, c, closest_d=1E10, max_c=0;

	*distance = *bearing = NULL;
	*name = NULL;

	ref.Lat = cur_pos_lat*(PI/180);
	ref.Lon = cur_pos_lon*(PI/180);
	ref.SinLat = sinf(ref.Lat);
	ref.CosLat = cosf(ref.Lat);
		
	strcpy(ref.Name, "Reference");

	while (strcmp(waypoints[i].Name, "END")) {
#if VER == 11 // Calculate distance
		d = Calc_Distance(&ref, &(waypoints[i]) );
		// if we found a closer waypoint, remember it and display it
		if (d<closest_d) {
			closest_d = d;
			closest_i = i;
		}
#else // Calculate proximity - how close is the point
		c = Calc_Proximity(&ref, &(waypoints[i]) );
		if (c>max_c) {
			max_c = c;
			closest_i = i;
		}
#endif
#if VER != 13
		b = Calc_Bearing(&ref, &(waypoints[i]) ); // Deletable!
#endif
		i++;
	}

	// Finish calcuations for the closest point
#if VER == 11
		d = Calc_Distance(&ref, &(waypoints[closest_i]) );
#else
	d = acosf(max_c)*6371; // finish distance calcuation
#endif	
	
	b = Calc_Bearing(&ref, &(waypoints[closest_i]) );
	// return information to calling function about closest waypoint 
	*distance = d;
	*bearing = b;
	*name = (char * ) (waypoints[closest_i].Name);
}
