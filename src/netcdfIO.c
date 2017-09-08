#include "jmap.h"


#define ERR(e) {printf("Error in netcdf load\n");}

void read_data(e *E){
    // printf("bathy filename = %s\n", E->b.fname);
    // printf("lat_name = %s\n", E->b.lat_name);

    // got the file info, lets read it into memory
    int ncid;
    int varid;
    int retval;

	int	i,j;

	float ***tmpField;


    // open the file
    if((retval = nc_open(E->input_file, NC_NOWRITE, &ncid)))
        ERR(retval);

    // get the lat dimension sizes
    if((retval = nc_inq_dimid(ncid, E->lat_name, &varid)))
        ERR(retval);

    if((retval = nc_inq_dimlen(ncid,varid,&E->nlat)))
        ERR(retval);

    printf("nlat = %d\n", E->nlat);

    // get the lon dimension sizes
    if((retval = nc_inq_dimid(ncid, E->lon_name, &varid)))
        ERR(retval);

    if((retval = nc_inq_dimlen(ncid,varid,&E->nlon)))
        ERR(retval);

    //cout << "nlats = " << nlat << endl;
    //cout << "nlons = " << nlon << endl;

    printf("nlon = %d\n", E->nlon);

    // malloc room for the arrays
    E->lat = malloc(E->nlat*sizeof(double));
    E->lon = malloc(E->nlon*sizeof(double));
    E->field = malloc2d_float(E->nlat, E->nlon);
	// for 3d data:
	tmpField = malloc3d_float(8,E->nlat, E->nlon);


    // read the data
    nc_inq_varid(ncid, E->lat_name, &varid);
    nc_get_var_float(ncid, varid, E->lat);

    nc_inq_varid(ncid, E->lon_name, &varid);
    nc_get_var_float(ncid, varid, E->lon);

		// check the bounds on the longitude array
		// and set the do_shiftgrid flag as appropriate
		if( (E->lon[0] < 0.0) )
			// our input data is already in the -180.0 to 180.0 longitude range
			// nothing to do
			E->do_shiftgrid = FALSE;
		else{
			// our input data is in the 0 to 360 longitude range
			E->do_shiftgrid = TRUE;
		}
			printf("data lon_min = %f, lon_max = %f\n", E->lon[0], E->lon[E->nlon-1]);
			printf("do_shiftgrid is %d\n", E->do_shiftgrid);

    nc_inq_varid(ncid, E->field_name, &varid);

		// see if there is a scale and offset value for this data


    //cout << " getting field: " << field_name.c_str() << endl;
    //nc_get_var_float(ncid, varid, &field[0][0]);

		// should change this to a get_vara call
		// to handle 2d, 3d and 4d cases (what about n-dimensional?)
    if((retval =nc_get_var_float(ncid, varid, &tmpField[0][0][0])))
        ERR(retval);



	for(i=0;i<E->nlat;i++){
		for(j=0;j<E->nlon;j++){
			E->field[i][j] = tmpField[0][i][j];
		}
	}

	free(tmpField);
		/*
		// process field to apply scale and offset
		for(i=0;i<E->nlat;i++){
			for(j=0;j<E->nlon;j++){
				E->field[i][j] *= scale_factor
			}
		}
		*/

		// make fill values equal to zero
		/*
		for(i=0;i<E->nlat;i++){
			for(j=0;j<E->nlon;j++){
				if(E->field[0][i][j] == -1.e+20f)
					E->field[0][i][j] = 0.0;
			}
		}
		*/


		// for oceanmaps data:
		
		if(E->do_shiftgrid = TRUE){
			// only works for global grids
			// need to check against the longitude array for longitudes > 180.0 and wrap around
			float **shifted = malloc2d_float(E->nlat, E->nlon);
			int	half = E->nlon/2;
			
			// artificial wrap for oceanmaps data
			E->lon[0] = 0.0;
			//for(j=0;j<half;j++){
			//	E->field[0][j] = E->field[E->nlat-1][j];
			//}
			// now remap the data array
			for(i=0;i<E->nlat;i++){
				for(j=0;j<half;j++){
					shifted[i][j+half] = E->field[i][j];
				}
			}
			// now remap the data array
			for(i=0;i<E->nlat;i++){
				for(j=half;j<E->nlon;j++){
					shifted[i][j-half] = E->field[i][j];
				}
			}

			// remap the longitude array
			for(i=0;i<E->nlon;i++){
				//if(E->lon[i] > 180.0 ) E->lon[i] = E->lon[i] - 360.0;
				E->lon[i] = E->lon[i] - 180.0;
			}
			// copy back
			for(i=0;i<E->nlat;i++){
				for(j=0;j<E->nlon;j++){
					E->field[i][j] = shifted[i][j]; 
				}
			}

			E->do_shiftgrid = FALSE;
		}
		


		/*
		// figure out min and max values
		E->min_val = 999.0;
		E->max_val = -999.0;
		for(i=0;i<E->nlat;i++){
			for(j=0;j<E->nlon;j++){

				if(E->field[i][j] > -999.0){
					if(E->field[i][j] < E->min_val)
						E->min_val = E->field[i][j];

					if(E->field[i][j] > E->max_val)
						E->max_val = E->field[i][j];
				}
			}
		}

		printf("min_val = %f\nmax_val = %f\n", E->min_val, E->max_val);
		*/
}
