// freeman.justin@gmail.com

#include "jmap.h"


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

int main(int argc,char **argv)
{
    e	*E;
    int i,j,k, z;
    int row,col;
    int index;
    int zoom_tiles;
    bounding_box box;

    //colormap cm;
    //float aspect_ratio;

    int   nx,ny;
    int   ii,jj;

    int start;
    int end;
    int chunk_size = 256;
    //int chunk_size = 512;
    
    int num_x_tiles;
    int num_y_tiles;

    char fname[256];

    float lon_scale;
    float lat_scale;

    struct stat st = {0};


	// malloc the work struct
	E = malloc(sizeof(e));
	if(E==NULL) fail("malloc failed\n");

	// parse command line arguments
	if(argc < 3){
		fail("need an input netcdf file and an output png filename\n");
	}
	else{
		get_command_line_arg_as_string(&E->input_file, argv[1]);
		get_command_line_arg_as_string(&E->fname, argv[2]);
	}

  E->max_zoom = 7;

  // read the netcdf data
  E->lat_name = malloc(80);
  E->lon_name = malloc(80);
  E->field_name = malloc(80);


  sprintf(E->lat_name,"yu_ocean");
  sprintf(E->lon_name,"xu_ocean");
  sprintf(E->field_name,"speed");


/*
  sprintf(E->lat_name,"lat");
  sprintf(E->lon_name,"lon");
  sprintf(E->field_name,"strs");
*/

  /*
  // bathymetry ETOPO1
  sprintf(E->lat_name,"lat");
  sprintf(E->lon_name,"lon");
  sprintf(E->field_name,"z");
  */

  /*
  // wavewatch3
  sprintf(E->lat_name,"lat");
  sprintf(E->lon_name,"lon");
  sprintf(E->field_name,"sig_wav_ht");
  */
  read_data(E);

  printf("done read data\n");


/* generate tile sets
256x256 tile images with the following format:
{out_dir}/{zoom}/{x}/{y}.png

zoom 0 (farthest)/0/0.png <-- 1 image
zoom 1  1/0/0.png <-- top left
        1/0/1.png
        1/1/0.png
        1/1/1.png <-- top right
*/

    //E->width = 1500;
    //E->height = 3600;

    // assusme coord are -180 to 180 for requests


    box.min_lat = E->lat[0];
    box.max_lat = E->lat[E->nlat-1];
    box.min_lon = E->lon[0];
    box.max_lon = E->lon[E->nlon-1];

    if(E->do_shiftgrid == TRUE){
      if(box.min_lon < 0.0){
        box.min_lon += 180.0;
        box.max_lon += 180.0;
      }
      E->longitude_offset = 0.0;
    }
    else{
      E->longitude_offset = 180.0;
    }

    // for yasi regional stress:
    //E->longitude_offset = 180.0;

    printf("*** min_lat = %f, max_lat = %f\n", E->lat[0], E->lat[E->nlat-1]);
    printf("*** min_lon = %f, max_lon = %f\n", E->lon[0], E->lon[E->nlon-1]);
    
    // for map projection bounds
    //box.min_lat = -85.0511;
    //box.max_lat = 85.0511;

    // for reprojection
   /* 
    E->mapLeft = floor(E->lon[0]);
    E->mapRight = ceil(E->lon[E->nlon-1]);
    E->mapTop = ceil(E->lat[E->nlat-1]);
    E->mapBottom = floor(E->lat[0]);
    */
    // these are the indexes into the data array
    box.lon_start = find_closest(E->lon, box.min_lon, 0, E->nlon-1);//floor(E->lon[0]);
    box.lon_end = find_closest(E->lon, box.max_lon, 0, E->nlon-1);//ceil(E->lon[E->nlon-1]);
    box.lat_start = find_closest(E->lat, box.min_lat, 0, E->nlat-1);//ceil(E->lat[E->nlat-1]);
    box.lat_end = find_closest(E->lat, box.max_lat, 0, E->nlat-1);//floor(E->lat[0]);

    // image dimensions
    int scale = 1;  // scale factor should be dynamically calculated
    E->width  = chunk_size;//(box.lon_end-box.lon_start)*scale;
    E->height = chunk_size;//(box.lat_end-box.lat_start)*scale;
    printf("width = %d, height = %d\n", E->width, E->height);

    // allocate memory for the image data
    // this is for a rgba png file
    if ((E->image = (char *) malloc(E->height*E->width*4*sizeof(char)) ) == NULL) {
        fail("failed to allocate memory for image \n");
    }

    // figure out min and max values
    E->min_val = 99999.0;
    E->max_val = -99999.0;
    for(i=box.lat_start;i<box.lat_end;i++){
      for(j=box.lon_start;j<box.lon_end;j++){

        // transform low
        E->field[i][j] = sqrt(E->field[i][j]);
        E->field[i][j] = sqrt(E->field[i][j]);

        // for oceanmaps
        
        if(E->field[i][j] > -999.0){
          if(E->field[i][j] < E->min_val)
            E->min_val = E->field[i][j];

          if(E->field[i][j] > E->max_val)
            E->max_val = E->field[i][j];
        }

        

      /*
        // for bathymetry
        if(E->field[i][j] < E->min_val)
            E->min_val = E->field[i][j];

        if(E->field[i][j] > E->max_val)
          E->max_val = E->field[i][j];
      */
        

    /*
       if(E->field[i][j] > -1000000.0){
          if(E->field[i][j] < E->min_val)
            E->min_val = E->field[i][j];

          if(E->field[i][j] > E->max_val)
            E->max_val = E->field[i][j];
        }
        */
      }
    }
   
    printf("%%%% min_val = %f\nmax_val = %f\n", E->min_val, E->max_val);


  /*
    // just do this for bathymetry data 
    // normalize range
    float range = E->max_val-E->min_val;
    for(i=box.lat_start;i<box.lat_end;i++){
      for(j=box.lon_start;j<box.lon_end;j++){
        E->field[i][j]/=range;
      }
    }

    // recalc range
    E->min_val = 99999.0;
    E->max_val = -99999.0;
    for(i=box.lat_start;i<box.lat_end;i++){
      for(j=box.lon_start;j<box.lon_end;j++){

        // transform low
       // E->field[i][j] = sqrt(E->field[i][j]);
        //E->field[i][j] = sqrt(E->field[i][j]);

        // for bathymetry
        if(E->field[i][j] < E->min_val)
            E->min_val = E->field[i][j];

        if(E->field[i][j] > E->max_val)
          E->max_val = E->field[i][j];
      }
    }
    */

    // for stress plot
    //E->min_val = 0.0;
    //E->max_val = 1.5;

    // for oceanmaps speed
    E->min_val = 0.0;
    E->max_val = 1.1;
    printf("&&& min_val = %f\nmax_val = %f\n", E->min_val, E->max_val);


    //aspect_ratio = (box.lon_end - box.lon_start)/(box.lat_end - box.lat_start);
    //printf("aspect ratio = %f\n", aspect_ratio);

    // initial sizing of zoom level 0
    // this works for large array data
    // but will fail with smaller (ie. regional) datasets
    // for zoom 0: divide by 16
    E->draw_width = chunk_size;//(box.lon_end - box.lon_start)/16.0;
    E->draw_height = chunk_size;//(box.lat_end - box.lat_start)/16.0;

    // zoom 1: each zoom level is 2x the size of the previous layer.
    // but need to tile this is x and y directions...
    //E->draw_width *=2;
    //E->draw_height *=2;

    // level = 0 --> 1 tile
    // level = 1 --> 2 tiles
    // level = 2 --> 4 tiles

    // dynamically determine number of tiles -- see chunk code below
    //zoom_tiles = 2;

    // split the tiles into 256x256 chunks
    // with the last tile having the remainder
    //printf("n xtile = %f\n", (int)E->draw_width/(int)256);
    //printf("n ytile = %f\n", (int)E->draw_height/(int)256);

    
    /*
    for (int i = 0; i < 2; i++) {
        int start = i * chunk_size;
        int end = start + chunk_size - 1;
        if (i == 2 - 1) {
            end = E->draw_width - 1;
        }

        printf("i = %d, start = %d, end = %d\n",i, start, end);
       
    }
    */
    //lon_scale = (float)(E->nlon-1)/(float)E->draw_width;
    //lat_scale = (float)E->nlat/(float)E->draw_height;
    //printf("lon_scale = %f\n", lon_scale);
    //printf("lat_scale = %f\n", lat_scale);
    
    
    // the tile generation loop
    // needs an outer loop to generate tile sets.
    printf("\n\n");
    for(z=0;z<=E->max_zoom;z++){

      printf("\nzoom_level = %d\n", z);
      if(z>0){
        E->draw_width  *= 2;
        E->draw_height *= 2;
      }
      printf("total image width = %d\n", E->draw_width);
      printf("total image height = %d\n", E->draw_height);

      lon_scale = (float)(E->nlon-1)/(float)E->draw_width;
      lat_scale = (float)(E->nlat-1)/(float)E->draw_height;
      printf("lon_scale = %f\n", lon_scale);
      printf("lat_scale = %f\n", lat_scale);

      
      // calculate the number of x tiles required
      // and the number of y tiles required
      num_x_tiles = ceil((float)E->draw_width/(float)chunk_size);
      num_y_tiles = ceil((float)E->draw_height/(float)chunk_size);
      printf("\tnumber of x tiles: %d\n", num_x_tiles);
      printf("\tnumber of y tiles: %d\n", num_y_tiles);
      
      // now spawn the directory structure
      char dir_name[1024];
      sprintf(E->dir_base,"oceanmaps_projected");
      for(j=0;j<num_x_tiles;j++){
        sprintf(dir_name,"%s/%d/%d", E->dir_base,z,j);
        mkdirRecursive(dir_name, S_IRWXU);
      }
      
      //for(k=0;k<zoom_tiles;k++){  // <-- this needs to be a nested loop over x and y
      for(i=0;i<num_x_tiles;i++){
        for(j=0;j<num_y_tiles;j++){
          // clear the tile image data to white/transparent
          wipe_tile(E);
          
          // calculate the current tile draw size
          // need to change this for lat and lon indexing
          E->start_x = i * chunk_size;
          E->end_x = E->start_x + chunk_size;//- 1;
          if (i == num_x_tiles - 1)
            E->end_x = E->draw_width;
          printf("i = %d, start_x = %d, end_x = %d\n",i, E->start_x, E->end_x);

          // for drawing tiles from the lower left up
          // this is not what leaflet or openlayers expects
          
          E->start_y = j * chunk_size;
          E->end_y = E->start_y + chunk_size;//- 1;
          if (j == (num_y_tiles - 1))
            E->end_y = E->draw_height;
          
          /*
          if(num_y_tiles > 1){
            // leaflet wants tles from the top to bottom
            // so flip the j index here....
            E->tile_height = E->end_y - E->start_y;
            E->end_y = E->draw_height - E->start_y;
            E->start_y = E->end_y - E->tile_height;
          }
          */

          printf("j = %d, start_y = %d, end_y = %d\n",j, E->start_y, E->end_y);
      
          // calculate the lon/lat dimensions for this tile
          E->mapLeft = E->lon[(int)(E->start_x*lon_scale)];
          E->mapRight = E->lon[(int)(E->end_x*lon_scale)];
          E->mapTop = E->lat[(int)(E->end_y*lat_scale)];
          E->mapBottom = E->lat[(int)(E->start_y*lat_scale)];
          printf("mapLeft index = %d\n", (int)(E->start_x*lon_scale));
          printf("mapRIght index = %d\n",(int)(E->end_x*lon_scale));
          printf("mapTop index = %d\n", (int)(E->end_y*lat_scale));
          printf("mapBottom index = %d\n",(int)(E->start_y*lat_scale));

          E->tile_width = E->end_x-E->start_x;
          E->tile_height = E->end_y-E->start_y;
          printf("this_width = %d\n", E->tile_width);
          printf("this_height = %d\n", E->tile_height);

          E->mapWidth    = E->tile_width;  // had E->width-1
          E->mapHeight   = E->tile_height; // had E->height-1

          E->lonDelta = (E->mapRight-E->mapLeft); // had a floor on this statement
          E->latDelta = (E->mapTop-E->mapBottom);  // had a ceil on this statement

          printf("width = %d\nheight = %d\n", E->width, E->height);
          printf("draw_width = %d\ndraw_height = %d\n", E->tile_width, E->tile_height);

          printf("west = %f\teast = %f\n", E->mapLeft, E->mapRight);
          printf("south = %f\tnorth = %f\n", E->mapBottom, E->mapTop);

          //cm = rainbow;
          draw_map(E, plasma, box);
          //draw_map_adjusted(E, rainbow, box);
          //draw_map(E,artmap_danflavin1,box);
          //draw_map(E,rainbow,box);


          // write this out with the correct y index
          sprintf(fname,"%s/%d/%d/%d.png", E->dir_base,z,i,j);
          stbi_write_png(fname, E->width, E->height, 4, (const void *)E->image, 0);
        }
      }
      
      printf("******************************\n\n");
    }

    write_leaflet(E);

    return 0;




    /*
    // resize to 256x256
    int scaled_width = 256;//E->width;
    int scaled_height = 256;//E->height;

    if( (scaled_width != E->width) && (scaled_height != E->height) ){
      char *scaled_image = (char *) malloc(scaled_width*scaled_height*4*sizeof(char));
      stbir_resize_uint8_srgb((const void *)E->image , E->width, E->height , 0,
                              (const void *)scaled_image, scaled_width, scaled_height, 0, 4, 3, 0);
      stbi_write_png(E->fname, scaled_width, scaled_height, 4, (const void *)scaled_image, 0);
    }
    else{
      // write the image
      stbi_write_png(E->fname, E->width, E->height, 4, (const void *)E->image, 0);
    }
    
	return 0;
  */
}
