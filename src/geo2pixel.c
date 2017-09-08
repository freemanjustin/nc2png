#include "jmap.h"

#define DEG2RAD(a)   ((a) / (180 / M_PI))
#define RAD2DEG(a)   ((a) * (180 / M_PI))
#define EARTH_RADIUS 6378137

void geo2pixel(e *E, float latitude, float longitude, int *y, int *x) {

  float mappedLon, mappedLat;

  float mapWidth    = E->width-1;
  float mapHeight   = E->height-1;

  float lonDelta = (E->mapRight-E->mapLeft); // had a floor on this statement
  float latDelta = (E->mapTop-E->mapBottom);  // had a ceil on this statement

  mappedLon = (longitude - E->mapLeft)*(mapWidth/lonDelta);
  mappedLat = mapHeight - (latitude-E->mapBottom)*(mapHeight/latDelta);

  *x = floor(mappedLon);
  *y = floor(mappedLat);

  return;

}

void pixel2geo(e *E, float *latitude, float *longitude, int y, int x) {

  *longitude = (float)x/(E->mapWidth/E->lonDelta) + E->mapLeft;
  *latitude = E->mapBottom-((float)y - E->mapHeight)/(E->mapHeight/E->latDelta);

  return;

}


// given a pixel pair find out which lat, lon pair this corresponds to
void pixel2geo_webMercator(e *E, float *latitude, float *longitude, int y, int x) {

//float y_shifted;

*longitude = ((360.0 * (float)(x+E->start_x)) / E->draw_width) - E->longitude_offset; 
//*longitude = (float)x/(E->mapWidth/E->lonDelta) + E->mapLeft;

printf("(total width = %d) pixel x = %d, longitude = %f\n", E->draw_width, x, *longitude);

       // double lng = (point.x - _pixelOrigin.x) / _pixelsPerLonDegree;
       // double latRadians = (point.y - _pixelOrigin.y) / - _pixelsPerLonRadian;
       // double lat = radiansToDegrees(2 * Math.atan(Math.exp(latRadians)) - Math.PI / 2);
       // return new PointF(lat, lng);

//float latRad = E->mapBottom-((float)y - E->mapHeight)/(E->mapHeight/E->latDelta);
//*latitude = RAD2DEG(2.0 * atan(exp(latRad)) - M_PI / 2.0);

// works for global maps
//*latitude = 90.0 * (-1.0 + (4.0 * atan(exp(M_PI - (2.0 * M_PI * ((float)y) / E->mapHeight)) ) / M_PI);

//*latitude = E->mapBottom-((float)y - E->mapHeight)/(E->mapHeight/E->latDelta);
*latitude = 90.0 * (-1.0 + (4.0 * atan(exp(M_PI - (2.0 * M_PI * ((float)y+E->start_y) / E->draw_height)) ) / M_PI));
// this is in -86 to 86 degrees....
// correct latitude base on the current tile lat dimensions

//long = ((360 * x) / mapWidth) - 180; 

//lat = 90 * (-1 + (4 * Math.atan(Math.pow(Math.E, (Math.PI - (2 * Math.PI * y) / mapHeight))) ) / Math.PI);

//printf("start_y = %d, pixel = %d,  lat = %f\n",E->start_y, y, *latitude);
  return;

}


// web mercator projections
// no scaling with map dimensions




/* The following functions take their parameter and return their result in degrees */

float y2lat_d(float y)   { return RAD2DEG( atan(exp( DEG2RAD(y) )) * 2 - M_PI/2 ); }

float y2lat_d_j(e *E, float y){ 
  //return RAD2DEG( atan(exp( DEG2RAD(y) )) * 2 - M_PI/2 ); 
  float lat = RAD2DEG( atan(exp( DEG2RAD(y) )) * 2 - M_PI/2 ); // get pixel latitude web mercator projection

  //lat = E->mapBottom-(lat - E->mapHeight)/(E->mapHeight/E->latDelta);


  printf("pixel = %f, lat = %f\n", y, lat);
  return lat;

  //return RAD2DEG(2.0 * ( atan(exp(DEG2RAD(y) - M_PI/4.0)) ));
}

float x2lon_d(float x)   { return x; }

float lat2y_d(float lat) { return RAD2DEG( log(tan( DEG2RAD(lat) / 2 +  M_PI/4 )) ); }
float lon2x_d(float lon) { return lon; }

/* The following functions take their parameter in something close to meters, along the equator, and return their result in degrees */

float y2lat_m(float y)   { return RAD2DEG(2 * atan(exp( y/EARTH_RADIUS)) - M_PI/2); }
float x2lon_m(float x)   { return RAD2DEG(              x/EARTH_RADIUS           ); }

/* The following functions take their parameter in degrees, and return their result in something close to meters, along the equator */

float lat2y_m(float lat) { return log(tan( DEG2RAD(lat) / 2 + M_PI/4 )) * EARTH_RADIUS; }
float lon2x_m(float lon) { return          DEG2RAD(lon)                 * EARTH_RADIUS; }
