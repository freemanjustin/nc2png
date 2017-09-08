#include "jmap.h"

void write_leaflet(e *E){

  char  str[1024];
  FILE  *out;

  sprintf(str,"%s/leaflet.html",E->dir_base );
  out = fopen(str,"w");
  if(out == NULL){
    fprintf(stderr,"Failed to open output HTML file\n");
    return;
  }

  fprintf(out," \n");
  fprintf(out,"<!DOCTYPE html>\n");
  fprintf(out,"<html>\n");
  fprintf(out,"  <head>\n");
  fprintf(out,"    <title>SuperSize</title>\n");
  fprintf(out,"    <meta charset=\"utf-8\"/>\n");
  fprintf(out,"    <meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no\"/>\n");
  fprintf(out,"    <link rel=\"stylesheet\" href=\"https://npmcdn.com/leaflet@1.0.0-rc.3/dist/leaflet.css\" />\n");
  fprintf(out,"    <script src=\"https://npmcdn.com/leaflet@1.0.0-rc.3/dist/leaflet.js\"></script>\n");
  fprintf(out,"    <script>\n");
  fprintf(out,"      var layer;\n");
  fprintf(out,"     function init() {\n");
  fprintf(out,"        var mapMinZoom = 0;\n");
  fprintf(out,"        var mapMaxZoom = %d;\n", E->max_zoom);
  fprintf(out,"        var map = L.map('map', {\n");
  fprintf(out,"         maxZoom: mapMaxZoom,\n");
  fprintf(out,"          minZoom: mapMinZoom,\n");
  fprintf(out,"          crs: L.CRS.Simple\n");
  fprintf(out,"        }).setView([0, 0], mapMaxZoom);\n");
  fprintf(out,"      \n");
  fprintf(out,"\n");
  fprintf(out,"        var mapBounds = new L.LatLngBounds(\n");
  fprintf(out,"            map.unproject([0, %d], mapMaxZoom),\n", E->draw_height);
  fprintf(out,"            map.unproject([%d, 0], mapMaxZoom));\n", E->draw_width);
  fprintf(out,"            \n");
  fprintf(out,"       map.fitBounds(mapBounds);\n");
  fprintf(out,"       layer = L.tileLayer('{z}/{x}/{y}.png', {\n");
  fprintf(out,"          minZoom: mapMinZoom, maxZoom: mapMaxZoom,\n");
  fprintf(out,"          bounds: mapBounds,\n");
  fprintf(out,"          attribution: 'Rendered with <i>SuperSize</i>',\n");
  fprintf(out,"          noWrap: true,\n");
  fprintf(out,"          tms: false\n");
  fprintf(out,"        }).addTo(map);\n");
  fprintf(out,"      }\n");
  fprintf(out,"    </script>\n");
  fprintf(out,"    <style>\n");
  fprintf(out,"      html, body, #map { width:100%%; height:100%%; margin:0; padding:0; z-index: 1; }\n");
  fprintf(out,"      #slider{ position: absolute; top: 10px; right: 10px; z-index: 5; }\n");
  fprintf(out,"    </style>\n");
  fprintf(out,"  </head>\n");
  fprintf(out,"  <body onload=\"init()\">\n");
  fprintf(out,"    <div id=\"map\"></div>\n");
  fprintf(out,"    <input id=\"slider\" type=\"range\" min=\"0\" max=\"1\" step=\"0.1\" value=\"1\" oninput=\"layer.setOpacity(this.value)\">\n");
  fprintf(out,"  </body>\n");
  fprintf(out,"</html>\n");

  return;

}
