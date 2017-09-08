#include "jmap.h"

/*
VERSION=version: Request version

REQUEST=GetMap: Request name

LAYERS=layer_list: Comma-separated list of one or more map layers. Optional if SLD parameter is present.

STYLES=style_list: Comma-separated list of one rendering style per requested layer. Optional if SLD parameter is present. Set “STYLES=” with an empty value to use default style(s). Named styles are also supported and are controlled by CLASS GROUP names in the mapfile.

SRS=namespace:identifier: Spatial Reference System.

BBOX=minx,miny,maxx,maxy: Bounding box corners (lower left, upper right) in SRS units.

WIDTH=output_width: Width in pixels of map picture.

HEIGHT=output_height: Height in pixels of map picture.

FORMAT=output_format: Output format of map.
*/

#define OUTSIDE -1

void draw_map(e *E, colormap cm, bounding_box box){

  int i,j;
  float y2lat, x2lon, nn;

  // create an array of lat and lon indices to lookup nearest neighbor info fast
  int *xlut, *ylut;
  float scale_x, scale_y;

  xlut = malloc(E->width*sizeof(int));
  ylut = malloc(E->height*sizeof(int));

  // create colormap lut
  init_cmaps_from_data(E, cm);

  for(j=0;j<E->height;j++){
    pixel2geo_webMercator(E, &y2lat, &x2lon, j, 0);
    if( (y2lat < E->lat[0]) || (y2lat > E->lat[E->nlat-1]) ){
      // this pixel is outside of data range
      // mark it to make transparent later
      ylut[j] = OUTSIDE;
    }
    else
      ylut[j] = find_closest(E->lat, y2lat, box.lat_start, box.lat_end);
  }

  for(i=0;i<E->width;i++){
    pixel2geo_webMercator(E, &y2lat, &x2lon, 0, i);
    if( (x2lon < E->lon[0]) || (x2lon > E->lon[E->nlon-1]) ){
      // this pixel is outside of data range
      // mark it to make transparent later
      xlut[i] = OUTSIDE;
    }
    else{
      // x2lon is the longitude of our map projected pixel
      // xlut[i] is the location of this point in our data array

      xlut[i] = find_closest(E->lon, x2lon, box.lon_start, box.lon_end);

/*
      // want to map points greater than 180 degrees to be -180
      // ie. we shift the location of these pixels to the left
      // of the image array

      // does box.lon bounds cross 180?
      if( (box.min_lon < 180.0) && (box.max_lon > 180)){
        //printf("doing shift grid\n");
        // shift indexes on bounds by 180 degrees
        // this means that everything greater than 180 degrees
        // the 180 to 360 degree stuff becomes -180 to 0 degrees

        // the variable shiftgrid_lon is the array index of the 180 degree pixel
        int shiftgrid_lon = find_closest(E->lon, 180.0, box.lon_start, box.lon_end);
        //printf("shiftgrid_lon = %d, lon_end = %d\n", shiftgrid_lon, box.lon_end);
        // shift the look up table index
        if(xlut[i] < shiftgrid_lon) xlut[i] = xlut[i] + shiftgrid_lon + 1;
        else xlut[i] = xlut[i] - shiftgrid_lon;
      }
*/
    }

  }


  for(j=0;j<E->height;j++){
    for(i=0;i<E->width;i++){

        if( (ylut[j] != OUTSIDE) && (xlut[i] != OUTSIDE) && (E->field[ylut[j]][xlut[i]] > -999.0) ){
          //printf("ylut[%d] = %d, xlut[%d] = %d\n",j,ylut[j],i,xlut[i]);
          apply_colormap(E, E->cmap, E->field[ylut[j]][xlut[i]], E->min_val, E->max_val );
          E->image[ (j * E->width  + i) * 4 + 0] = (int)E->r;//(int)get_red(E->field[0][(E->nlat-1)-j][i], E->min_val, E->max_val);
          E->image[ (j * E->width  + i) * 4 + 1] = (int)E->g;//(int)get_red(E->field[0][(E->nlat-1)-j][i], E->min_val, E->max_val);
          E->image[ (j * E->width  + i) * 4 + 2] = (int)E->b;//(int)get_red(E->field[0][(E->nlat-1)-j][i], E->min_val, E->max_val);
          E->image[ (j * E->width  + i) * 4 + 3] = 255;
        }
        else{ // make fill value transparent
          E->image[ (j * E->width  + i) * 4 + 0 ] = 255;
          E->image[ (j * E->width  + i) * 4 + 1] = 255;
          E->image[ (j * E->width  + i) * 4 + 2] = 255;
          E->image[ (j * E->width  + i) * 4 + 3] = 0;
        }

    }
  }
  free(xlut);
  free(ylut);

}


void draw_map_adjusted(e *E, colormap cm, bounding_box box){

  int i,j;
  float y2lat, x2lon, nn;

  // create an array of lat and lon indices to lookup nearest neighbor info fast
  int *xlut, *ylut;
  float scale_x, scale_y;

  xlut = malloc(E->tile_width*sizeof(int));
  ylut = malloc(E->tile_height*sizeof(int));

  // create colormap lut
  init_cmaps_from_data(E, cm);

  for(j=0;j<E->tile_height;j++){
    pixel2geo(E, &y2lat, &x2lon, j, 0);
    if( (y2lat < E->lat[0]) || (y2lat > E->lat[E->nlat-1]) ){
      // this pixel is outside of data range
      // mark it to make transparent later
      ylut[j] = OUTSIDE;
    }
    else
      ylut[j] = find_closest(E->lat, y2lat, box.lat_start, box.lat_end);
  }

  for(i=0;i<E->tile_width;i++){
    pixel2geo(E, &y2lat, &x2lon, 0, i);
    if( (x2lon < E->lon[0]) || (x2lon > E->lon[E->nlon-1]) ){
      // this pixel is outside of data range
      // mark it to make transparent later
      xlut[i] = OUTSIDE;
    }
    else{
      // x2lon is the longitude of our map projected pixel
      // xlut[i] is the location of this point in our data array

      xlut[i] = find_closest(E->lon, x2lon, box.lon_start, box.lon_end);

/*
      // want to map points greater than 180 degrees to be -180
      // ie. we shift the location of these pixels to the left
      // of the image array

      // does box.lon bounds cross 180?
      if( (box.min_lon < 180.0) && (box.max_lon > 180)){
        //printf("doing shift grid\n");
        // shift indexes on bounds by 180 degrees
        // this means that everything greater than 180 degrees
        // the 180 to 360 degree stuff becomes -180 to 0 degrees

        // the variable shiftgrid_lon is the array index of the 180 degree pixel
        int shiftgrid_lon = find_closest(E->lon, 180.0, box.lon_start, box.lon_end);
        //printf("shiftgrid_lon = %d, lon_end = %d\n", shiftgrid_lon, box.lon_end);
        // shift the look up table index
        if(xlut[i] < shiftgrid_lon) xlut[i] = xlut[i] + shiftgrid_lon + 1;
        else xlut[i] = xlut[i] - shiftgrid_lon;
      }
*/
    }

  }


  for(j=0;j<E->tile_height;j++){
    for(i=0;i<E->tile_width;i++){

        if( (ylut[j] != OUTSIDE) && (xlut[i] != OUTSIDE) && (E->field[ylut[j]][xlut[i]] >= E->min_val) ){
          //printf("ylut[%d] = %d, xlut[%d] = %d\n",j,ylut[j],i,xlut[i]);
          apply_colormap(E, E->cmap, E->field[ylut[j]][xlut[i]], E->min_val, E->max_val );
          E->image[ (j * E->width  + i) * 4 + 0] = (int)E->r;//(int)get_red(E->field[0][(E->nlat-1)-j][i], E->min_val, E->max_val);
          E->image[ (j * E->width  + i) * 4 + 1] = (int)E->g;//(int)get_red(E->field[0][(E->nlat-1)-j][i], E->min_val, E->max_val);
          E->image[ (j * E->width  + i) * 4 + 2] = (int)E->b;//(int)get_red(E->field[0][(E->nlat-1)-j][i], E->min_val, E->max_val);
          E->image[ (j * E->width  + i) * 4 + 3] = 0;
        }
        else{ // make fill value transparent
          E->image[ (j * E->width  + i) * 4 + 0 ] = 255;
          E->image[ (j * E->width  + i) * 4 + 1] = 255;
          E->image[ (j * E->width  + i) * 4 + 2] = 255;
          E->image[ (j * E->width  + i) * 4 + 3] = 255;
        }

    }
  }
  free(xlut);
  free(ylut);

}
