// freeman.justin@gmail.com

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// netCDF header
#include "netcdf.h"

#include "jutil.h"

// colormaps
// static defined
#include "colormaps_bright.h"
#include "colormaps_banded.h"
#include "colormaps_rainbow.h"
#include "colormaps_jaisnb.h"
#include "colormaps_jaisnc.h"
#include "colormaps_jaisnd.h"
#include "colormaps_blu_red.h"
#include "colormaps_manga.h"
#include "colormaps_jet.h"
#include "colormaps_wheel.h"

/* the following are original colormaps from ncview */
#include "colormaps_3gauss.h"
#include "colormaps_3saw.h"
#include "colormaps_bw.h"
#include "colormaps_default.h"
#include "colormaps_detail.h"
#include "colormaps_extrema.h"
#include "colormaps_helix.h"
#include "colormaps_helix2.h"
#include "colormaps_hotres.h"
#include "colormaps_ssec.h"

// jcoolormaps
#include "magma.h"
#include "inferno.h"
#include "plasma.h"
#include "viridis.h"
#include "parula.h"

// artmaps
#include "artmap.h"
#include "artmap_starry_night.h"
#include "pauldavies1.h"
#include "pauldavies2.h"
#include "pauldavies3.h"
#include "pauldavies4.h"
#include "pauldavies5.h"
#include "pauldavies6.h"
#include "pauldavies7.h"
#include "pauldavies8.h"

#include "jacksonpollock1.h"
#include "jacksonpollock2.h"
#include "jacksonpollock3.h"

#include "edwardhopper1.h"
#include "edwardhopper2.h"

#include "jeffwall1.h"

#include "danflavin1.h"
#include "danflavin2.h"

#include "gerhardrichter1.h"

#include "warhol1.h"
#include "warhol2.h"

#include "georgebyrne1.h"
#include "georgebyrne2.h"

#include "arthurboyd1.h"
#include "charlesdemuth1.h"
#include "claudemonet1.h"
#include "ellswothkelly1.h"
#include "franzkline1.h"
#include "jasperjohns1.h"
#include "joanmiro1.h"
#include "johnolsen1.h"
#include "johnolsen2.h"
#include "leilajeffreys1.h"
#include "leilajeffreys2.h"
#include "maxernst1.h"
#include "maxernst2.h"
#include "maxernst3.h"
#include "maxernst4.h"
#include "pauldelvaux1.h"
#include "pauldelvaux2.h"
#include "pauldelvaux3.h"
#include "pauldelvaux4.h"
#include "pauldelvaux5.h"
#include "renemagritte1.h"
#include "renemagritte2.h"
#include "rossbleckner1.h"
#include "roylichtenstein1.h"
#include "roylichtenstein2.h"
#include "yvestanguy1.h"
#define fail(...) jmap_fail(__LINE__,__func__,__FILE__,__VA_ARGS__)

#define pi  (M_PI)
#define earthradius (6356750.52)
#define deg2rad (M_PI/180.0)
#define rad2deg (180.0/M_PI)

#define max(x,y) (((x) > (y)) ? (x) : (y))
#define min(x,y) (((x) < (y)) ? (x) : (y))

#define FALSE (0)
#define TRUE  (1)

typedef struct{
  int r[256];
  int g[256];
  int b[256];
}cmap;

typedef enum {
  gauss3,
  detail,
  ssec,
  bright,
  banded,
  rainbow,
  jaisnb,
  jaisnc,
  jaisnd,
  blu_red,
  manga,
  jet,
  wheel,
  saw3,
  bw,
  defaultncview,
  extrema,
  helix,
  helix2,
  hotres,
  magma,
  inferno,
  plasma,
  viridis,
  parula,
  artmap,
  artmap_starry_night,
  artmap_pauldavies1,
  artmap_pauldavies2,
  artmap_pauldavies3,
  artmap_pauldavies4,
  artmap_pauldavies5,
  artmap_pauldavies6,
  artmap_pauldavies7,
  artmap_pauldavies8,
  artmap_jacksonpollock1,
  artmap_jacksonpollock2,
  artmap_jacksonpollock3,
  artmap_edwardhopper1,
  artmap_edwardhopper2,
  artmap_jeffwall1,
  artmap_danflavin1,
  artmap_danflavin2,
  artmap_gerhardrichter1,
  artmap_warhol1,
  artmap_warhol2,
  artmap_georgebyrne1,
  artmap_georgebyrne2,
  artmap_arthurboyd1,
  artmap_charlesdemuth1,
  artmap_claudemonet1,
  artmap_ellswothkelly1,
  artmap_franzkline1,
  artmap_jasperjohns1,
  artmap_joanmiro1,
  artmap_johnolsen1,
  artmap_johnolsen2,
  artmap_leilajeffreys1,
  artmap_leilajeffreys2,
  artmap_maxernst1,
  artmap_maxernst2,
  artmap_maxernst3,
  artmap_maxernst4,
  artmap_pauldelvaux1,
  artmap_pauldelvaux2,
  artmap_pauldelvaux3,
  artmap_pauldelvaux4,
  artmap_pauldelvaux5,
  artmap_renemagritte1,
  artmap_renemagritte2,
  artmap_rossbleckner1,
  artmap_roylichtenstein1,
  artmap_roylichtenstein2,
  artmap_yvestanguy1
}colormap;

typedef struct{

  int   lat_start;
  int   lat_end;
  int   lon_start;
  int   lon_end;
  float min_lat;
  float max_lat;
  float min_lon;
  float max_lon;

}bounding_box;

typedef struct {

    char*   input_string;
    char*   fname;
    char*   input_file;

    char*   lat_name;
    char*   lon_name;
    char*   field_name;

    float   mapLeft;
    float   mapRight;
    float   mapTop;
    float   mapBottom;

    int     mapWidth;
    int     mapHeight;

    int     start_x;
    int     end_x;
    int     start_y;
    int     end_y;
    

    float   lonDelta;
    float   latDelta;

    int     width;
    int     height;
    char*   image;

    int     draw_width;
    int     draw_height;

    int     tile_width;
    int     tile_height;

    float     r;
    float     g;
    float     b;

    size_t  nlat;
    size_t  nlon;
    float *lat;
    float *lon;
    float **field;
    float min_val;
    float max_val;

    char  dir_base[1024];

    cmap  cmap;

    int   do_shiftgrid;
    int max_zoom;
    float longitude_offset;

}e;

// prototypes
void jmap_fail( const int, const char*, const char*, const char*, ... );

// netcdf functions
void read_data( e* );


// map projection functions;
void geo2pixel(e *E, float latitide, float longitude, int *x, int *y);
void pixel2geo(e *E, float *latitude, float *longitude, int y, int x) ;
//void geo2pixel_webMercator(e *E, float latitude, float longitude, int *y, int *x);
void pixel2geo_webMercator(e *E, float *latitude, float *longitude, int y, int x);
float y2lat_d(float y);
float x2lon_d(float x);
float lat2y_d(float lat);
float lon2x_d(float lon);
float y2lat_m(float y);
float x2lon_m(float x);
float lat2y_m(float lat);
float lon2x_m(float lon);

float y2lat_d_j(e*,float y);

//search
int midpoint(int low, int high);
int binary_search(int A[], int key, int imin, int imax);
int find_closest_int(int A[], int key, int imin, int imax);
int find_closest(float *A, float key, int imin, int imax);

//colormaps
void init_cmaps_from_data(e *E, colormap cm);
void init_cmap_from_data( int *data, cmap *cmap );
void apply_colormap(e *E, cmap colormap, float value, float min_val, float max_val );

// draw_map
void draw_map(e *E, colormap cm, bounding_box box);
void draw_map_adjusted(e *E, colormap cm, bounding_box box);


// utils
void wipe_tile(e*);
void mkdirRecursive(const char *path, mode_t mode);

void write_leaflet(e *E);


