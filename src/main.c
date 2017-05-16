// freeman.justin@gmail.com

#include "jmap.h"


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

int main(int argc,char **argv)
{
    e	*E;
    int i,j;
    int row,col;
    int index;

    bounding_box box;

    //colormap cm;


    int   nx,ny;
    int   ii,jj;


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



  // read the netcdf data
  E->lat_name = malloc(80);
  E->lon_name = malloc(80);
  E->field_name = malloc(80);

  //sprintf(E->lat_name,"yt_ocean");
  //sprintf(E->lon_name,"xt_ocean");
  //sprintf(E->field_name,"eta_t");

  sprintf(E->lat_name,"yt_ocean");
  sprintf(E->lon_name,"xt_ocean");
  sprintf(E->field_name,"sst");

  read_data(E);

  printf("done read data\n");

    //E->width = 1500;
    //E->height = 3600;

    // assusme coord are -180 to 180 for requests
    //box.min_lat = -90.0;
    //box.max_lat = 90.0;
    //box.min_lon = -180.0;
    //box.max_lon = 180.0;

    //box.min_lat = -48.0;//E->lat[0];
    //box.max_lat = -46.0;//E->lat[E->nlat-1];
    //box.min_lon = 143.0;//E->lon[0];
    //box.max_lon = 148.0;//E->lon[E->nlon-1];

    box.min_lat = E->lat[0];
    box.max_lat = E->lat[E->nlat-1];
    box.min_lon = E->lon[0];
    box.max_lon = E->lon[E->nlon-1];


    if(E->do_shiftgrid == TRUE){
      if(box.min_lon < 0.0){
        box.min_lon += 180.0;
        box.max_lon += 180.0;
      }
    }



/*    
    box.min_lat = -40.0;
    box.max_lat = -20.0;
    box.min_lon = 150.0;
    box.max_lon = 190.0;
  */  

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
    E->width  = (box.lon_end-box.lon_start)*scale;
    E->height = (box.lat_end-box.lat_start)*scale;
    printf("width = %d, height = %d\n", E->width, E->height);

    // map bounds for the drawing function
    E->mapLeft = box.min_lon;//E->lon[box.lon_start];//floor(E->lon[0]);
    E->mapRight = box.max_lon;//E->lon[box.lon_end];//ceil(E->lon[E->nlon-1]);
    E->mapTop = box.max_lat;//E->lat[box.lat_end];//ceil(E->lat[E->nlat-1]);
    E->mapBottom = box.min_lat;//E->lat[box.lat_start];//floor(E->lat[0]);

    E->mapWidth    = E->width;  // had E->width-1
    E->mapHeight   = E->height; // had E->height-1

    E->lonDelta = (E->mapRight-E->mapLeft); // had a floor on this statement
    E->latDelta = (E->mapTop-E->mapBottom);  // had a ceil on this statement

    printf("width = %d\nheight = %d\n", E->width, E->height);

    printf("west = %f\teast = %f\n", E->mapLeft, E->mapRight);
    printf("south = %f\tnorth = %f\n", E->mapBottom, E->mapTop);

    // allocate memory for the image data
    // this is for a rgba png file
    if ((E->image = (char *) malloc(E->height*E->width*4*sizeof(char)) ) == NULL) {
        fail("failed to allocate memory for image \n");
    }

    // figure out min and max values
		E->min_val = 999.0;
		E->max_val = -999.0;
		for(i=box.lat_start;i<box.lat_end;i++){
			for(j=box.lon_start;j<box.lon_end;j++){

				if(E->field[i][j] > -999.0){
					if(E->field[i][j] < E->min_val)
						E->min_val = E->field[i][j];

					if(E->field[i][j] > E->max_val)
						E->max_val = E->field[i][j];
				}
			}
		}

    //E->min_val = 0.0;
    //E->max_val = 1.0;
		printf("min_val = %f\nmax_val = %f\n", E->min_val, E->max_val);

    //cm = rainbow;
    draw_map(E, inferno, box);

    stbi_write_png(E->fname, E->width, E->height, 4, (const void *)E->image, 0);
    return 0;

    /*
    // resize to 256x256
    int scaled_width = E->width;
    int scaled_height = E->height;

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
    */
	return 0;
}
