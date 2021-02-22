#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "dyad.h"
#include "device_utils.h"
#include "system_logger.h"
#include "http_server.h"
#include "device_settings.h"



char HTML_PAGE_HEAD[] = "<!DOCTYPE html>"
"<html lang='en'>"
"<head>"
"  <title>Multicon</title>"
"  <meta charset='utf-8'>"
"  <meta name='viewport' content='width=device-width, initial-scale=1'>"
"  <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css'>"
"  <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js'></script>"
"  <script src='https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.16.0/umd/popper.min.js'></script>"
"  <script src='https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js'></script>"
"  <style>"
"    /* Remove the navbar's default margin-bottom and rounded borders */ "
"    .navbar {margin-bottom: 0;border-radius: 0;}"
"    /* Set height of the grid so .sidenav can be 100% (adjust as needed) */"
"    .row.content {height: 450px}"
"    /* Set gray background color and 100% height */"
"    .sidenav {padding-top: 20px; background-color: #f1f1f1; height: 100%;}"
"    /* Set black background color, white text and some padding */"
"    footer {background-color: #555; color: white; padding: 15px;}"
"    /* On small screens, set height to 'auto' for sidenav and grid */"
"    @media screen and (max-width: 767px) {.sidenav {  height: auto;  padding: 15px;}.row.content {height:auto;} }"
"  </style>"
"</head>"
"<body>";

/* char HTML_PAGE_NAVBAR[] = "<nav class='navbar navbar-expand-lg navbar-dark bg-dark'>"
"  <!-- Brand/logo -->"
"  <a class='navbar-brand' href='/'>"
"     <div style='display: flex; font-weight: 900;'><div style='color: #007bff;'>MULTI</div><div>CON</div></div>"
"  </a>"
"  <!-- Links -->"
"  <ul class='navbar-nav'>"
"    <li class='nav-item'>"
"      <a class='nav-link' href='/log'>LOG</a>"
"    </li>"
"    <li class='nav-item'>"
"      <a class='nav-link' href='/share'>SHARE</a>"
"    </li>"
"    <li class='nav-item'>"
"      <a class='nav-link' href='/settings'>SETTINGS</a>"
"    </li>"
"  </ul>"
"  <script>"
"    $(document).ready(function() {"
"      if(window.location.pathname !=\"/\") {"
"         $('.navbar-nav').find('a[href*=\"'+window.location.pathname+'\"]').parent().addClass('active');"
"      }"
"    });"
"  </script>"
"</nav>"; */

char HTML_PAGE_NAVBAR[] =
"<nav class='navbar navbar-expand-lg navbar-dark bg-dark'>"
"  <a class='navbar-brand' href='/'>"
"     <div style='display: flex; font-weight: 900;'><div style='color: #007bff;'>MULTI</div><div>CON</div></div>"
"  </a>"
"  <button class='navbar-toggler' type='button' data-toggle='collapse' data-target='#navbarTogglerDemo02' aria-controls='navbarTogglerDemo02' aria-expanded='false' aria-label='Toggle navigation'>"
"    <span class='navbar-toggler-icon'></span>"
"  </button>"
"  <div class='collapse navbar-collapse' id='navbarTogglerDemo02'>"
"    <ul class='navbar-nav mr-auto mt-2 mt-lg-0'>"
"      <li class='nav-item'>"
"        <a class='nav-link' href='/log'>LOG</a>"
"      </li>"
"      <li class='nav-item'>"
"        <a class='nav-link' href='/share'>SHARE</a>"
"      </li>"
"      <li class='nav-item'>"
"        <a class='nav-link' href='/settings'>SETTINGS</a>"
"      </li>"
"    </ul>"
"  </div>"
"  <script>"
"    $(document).ready(function() {"
"      if(window.location.pathname !=\"/\") {"
"         $('.navbar-nav').find('a[href*=\"'+window.location.pathname+'\"]').parent().addClass('active');"
"      }"
"    });"
"  </script>"
"</nav>";



char HTML_PAGE_FOOTER[] = "<footer class=\"footer bg-dark\">"
"      <div class=\"container\">"
"        <span class=\"text-muted\">MULTICON 2020</span>"
"      </div>"
"    </footer>";

static int count = 0;


static void handle_home_page(dyad_Event *e)
{
    char html[10000];
    char HTML_PAGE_BODY1[] = "<div class='container-fluid text-center'>"
    "<div class='row content'>"
    "  <div class='col-sm-1 sidenav'>"
    "  </div>"
    "  <div class='col-sm-10 text-left'> "
    "    <p></p>"
    "    <h1>WELCOME TO MULTICON</h1>"
    "    <hr>"
    "    <h3>System Status</h3>"
    "       <div class='container-fluid'>"
    "           <table class='table table-striped'>"
    "           <thead>"
    "               <tr>"
    "               <th></th>"
    "               <th></th>"
    "               </tr>"
    "           </thead>"
    "           <tbody>";
    char temp[3000] = {'\0'};
    
    util_snprintf( temp, sizeof(temp),
        "<tr> <td>Serial</td>             <td>%s</td> </tr>\
        <tr> <td>Firmware</td>            <td>%s</td> </tr>\
        <tr> <td>Lan Ip</td>              <td>%s</td> </tr>\
        <tr> <td>Lan Netmask</td>         <td>%s</td> </tr>\
        <tr> <td>Lan Proto</td>           <td>%s</td> </tr>\
        <tr> <td>Zerotier Id</td>         <td>%s</td> </tr>\
        <tr> <td>Zerotier Join</td>       <td>%s</td> </tr>\
        <tr> <td>Zerotier Vpn</td>        <td>%s</td> </tr>\
        <tr> <td>Ip Address</td>          <td>%s</td> </tr>\
        <tr> <td>Gsm Rssi Level</td>      <td>%s</td> </tr>\
        <tr> <td>Wcdma Rscp Level</td>    <td>%s</td> </tr>\
        <tr> <td>Wcdma Ecio Level</td>    <td>%s</td> </tr>\
        <tr> <td>Lte Rsrp Level</td>      <td>%s</td> </tr>\
        <tr> <td>Lte Sinr Level</td>      <td>%s</td> </tr>\
        <tr> <td>Connection State</td>    <td>%s</td> </tr>\
        <tr> <td>Network State</td>       <td>%s</td> </tr>\
        <tr> <td>Sim State</td>           <td>%s</td> </tr>\
        <tr> <td>Sim Iccd</td>            <td>%s</td> </tr>",
        deviceSettings.serial,
        deviceSettings.firmware,
        deviceSettings.lan_ip,
        deviceSettings.lan_netmask,
        deviceSettings.lan_proto,
        deviceSettings.zerotier_id,
        deviceSettings.zerotier_join,
        deviceSettings.zerotier_vpnenabled,
        deviceSettings.ip_addr,
        deviceSettings.gsm_rssi_level,
        deviceSettings.wcdma_rscp_level,
        deviceSettings.wcdma_ecio_level,
        deviceSettings.lte_rsrp_level,
        deviceSettings.lte_sinr_level,
        deviceSettings.connstate,
        deviceSettings.netstate,
        deviceSettings.simstate,
        deviceSettings.sim_iccd);
    
    
    char HTML_PAGE_BODY2[] = "           </tbody>"
    "           </table>"
    "       </div>"
    "  </div>"
    "  <div class='col-sm-1 sidenav'>"
    "  </div>"
    "</div>"
    "</div>";
    util_snprintf(html, sizeof(html), "%s %s %s %s %s", HTML_PAGE_HEAD, HTML_PAGE_NAVBAR, HTML_PAGE_BODY1, temp, HTML_PAGE_BODY2, HTML_PAGE_FOOTER);
    dyad_writef(e->stream,  html);
}


static void handle_log_page(dyad_Event *e)
{
    char html[80000];
    char HTML_PAGE_BODY[] = "<div class='container-fluid text-center'>"
        "<div class='row content'>"
        "  <div class='col-sm-1 sidenav'>"
        "  </div>"
        "  <div class='col-sm-10 text-left'> "
        "    <style>"
        "        .text-info    { font-weight: bold!important; color: black}"
        "        .text-primary { font-weight: bold!important; color: #007bff!important}"
        "        .text-danger  { font-weight: bold!important; color: #dc3545!important}"
        "        .text-danger-nobold  { color: #dc3545!important}"
        "        .text-warning { font-weight: bold!important; color: #ffc107!important}"
        "    </style>"
        "    <p></p>"
        "      <input class='form-control' id='myInput' type='text' placeholder='Search..'>"
        "      <br>"
        "      <table class='table table-striped'>"
        "      <thead>"
        "          <tr>"
        "          <th>Timestamp</th>"
        "          <th>Module</th>"
        "          <th>Level</th>"
        "          <th>Message</th>"
        "          </tr>"
        "      </thead>"
        "      <tbody>"
        "      </tbody>"
        "      </table>"
        "  </div>"
        "</div>"
        "<script>"
        "    $(document).ready(function() { "
        "            $(\"tbody\").html(''); /*Remove elements*/"
        "            /* Get elements */"
        "            var months = [\"01\", \"02\", \"03\", \"04\", \"05\", \"06\", \"07\", \"08\", \"09\", \"10\", \"11\", \"12\"];"
        "            var date = new Date();"
        "            var y = date.getFullYear();"
        "            var m = months[date.getMonth()];"
        "            var d = date.getDate();"
        "            var filename = \"log/multicon_log_\"+y+\"_\"+m+\"_\"+d+\".csv\";"
        "            $.ajax({"
        "                type: \"GET\","
        "                url: filename,"
        "                dataType: \"text\","
        "                success: function(data) {processData(data);},"
        "                error: function () {console.log(\"File log not found\")}"
        "            });"
        "            /* Filter elements */"
        "            $(\"#myInput\").on(\"keyup\", function() {"
        "                var value = $(this).val().toLowerCase();"
        "                $(\"table tr\").filter(function() {"
        "                $(this).toggle($(this).text().toLowerCase().indexOf(value) > -1)"
        "                });"
        "            });"
        "        });"
        "        function processData(allText) {"
        "         console.log(allText); "
        "            var allTextLines = allText.split(/\\r\\n|\\n/);"
        "            var lines = [];"
        "            for (var i=1; i<allTextLines.length-1; i++) {"
        "                var data = allTextLines[i].split(';');"
        "                var timestamp = data[0];"
        "                var module = data[1];"
        "                var level = data[2].replace(' ','');"
        "                var mess = data[3];"
        "                var html = \"<tr>\";"
        "                html += \"<td>\" + timestamp + \"</td>\"; /* Timestamp */"
        "                html += \"<td>\" + module + \"</td>\"; /* Module */"
        "                if(level==\"INFO\")  { html += \"<td class='text-info'>\" + level + \"</td>\"; }"
        "                if(level==\"DEBUG\") { html += \"<td class='text-primary'>\" + level + \"</td>\"; }"
        "                if(level==\"ERROR\") { html += \"<td class='text-danger'>\" + level + \"</td>\"; }"
        "                if(level==\"WARN\")  { html += \"<td class='text-warning'>\" + level + \"</td>\"; }"
        "                if( (mess == \" Start Multicon device daemon\") || (mess == \" Multicon process was aborted abnormally, now it will restarted\") )"
        "                {"
        "                  html += \"<td class='text-danger-nobold '>\" + mess + \"</td>\"; /* Log Message */"
        "                }"
        "                else "
        "                {"
        "                  html += \"<td>\" + mess + \"</td>\"; /* Log Message */"
        "                }"
        "                html += \"</tr>\";"
        "                $(\"tbody\").append(html);"
        "            }"
        "        }"
        "</script>";
    
    util_snprintf(html, sizeof(html), "%s %s %s", HTML_PAGE_HEAD, HTML_PAGE_NAVBAR, HTML_PAGE_BODY);
    dyad_writef(e->stream,  html);
}

/* const char* getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ";");
            tok && *tok;
            tok = strtok(NULL, ";\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
} */
// TODO Implementare selezione del log
static void handle_log_content_page(dyad_Event *e, char* path)
{
    static char filename[100];
    char filename_timestamp[50]; 
    char timestamp[50]; 
    time_t ts;
    struct tm* ts_info;
       
    // Costruisco la stringa del timestamp
    ts = time(NULL);
    ts_info = localtime(&ts);
    
    // Costruisco la stringa del timestamp per il file di log
    strftime(filename_timestamp, 26, "%Y_%m_%d", ts_info);

    // Costruisco la stringa del filename
    util_snprintf(filename, sizeof(filename), "%s_%s.csv", "multicon_log", filename_timestamp);
  
    FILE* stream = fopen(filename, "r");
    if (stream == NULL)
    {
        system_logger(LOGGER_ERROR,"WEBSRV", "Error while opening the file %s", filename);
    }

    dyad_writef(e->stream, "%r", stream);
    fclose(stream);
}

static void handle_share_page(dyad_Event *e)
{
    char html[10000];
    char HTML_PAGE_BODY[] = "<div class='container-fluid text-center'>"
    "  <div class='row content'>"
    "    <div class='col-sm-1 sidenav'>"
    "    </div>"
    "    <div class='col-sm-10 text-left'> "
    "      <p></p>"
    "      <h1>Share Connection</h1>"
    "      <hr>"
    "      <h3>Under Construction</h3>"
    "      <p></p>"
    "      <p></p>"
    "      <p></p>"
    "      <p></p>"
    "      <p></p>"
    "      <p></p>"
    "      <p></p>"
    "    </div>"
    "    <div class='col-sm-1 sidenav'>"
    "    </div>"
    "  </div>"
    "</div>;";
    util_snprintf(html, sizeof(html), "%s %s %s %s", HTML_PAGE_HEAD, HTML_PAGE_NAVBAR, HTML_PAGE_BODY, HTML_PAGE_FOOTER);
    dyad_writef(e->stream,  html);
}


static void handle_settings_page(dyad_Event *e)
{
    char html[10000];
    char HTML_PAGE_BODY[] = "<div class='container-fluid text-center'>"
    "  <div class='row content'>"
    "    <div class='col-sm-1 sidenav'>"
    "    </div>"
    "    <div class='col-sm-10 text-left'> "
    "      <p></p>"
    "      <h1>Device Settings</h1>"
    "      <hr>"
    "      <div class='row'>"
    "          <div class='col-sm-2'>Log Level</div>"
    "          <div class='col-sm-5'>"
    "              <div class='dropdown'>"
    "                <button type='button' class='btn btn-primary dropdown-toggle' data-toggle='dropdown'>"
    "                  Dropdown button"
    "                </button>"
    "                <div class='dropdown-menu'>"
    "                  <a class='dropdown-item' href='#'>Link 1</a>"
    "                  <a class='dropdown-item' href='#'>Link 2</a>"
    "                  <a class='dropdown-item' href='#'>Link 3</a>"
    "                </div>"
    "              </div>"
    "          </div>"
    "          <div class='col-sm-5'></div>"
    "      </div>"
    "      <p></p>"
    "      <p></p>"
    "      <p></p>"
    "      <p></p>"
    "      <p></p>"
    "      <p></p>"
    "      <p></p>"
    "    </div>"
    "    <div class='col-sm-1 sidenav'>"
    "    </div>"
    "  </div>"
    "</div>;";
    util_snprintf(html, sizeof(html), "%s %s %s %s", HTML_PAGE_HEAD, HTML_PAGE_NAVBAR, HTML_PAGE_BODY, HTML_PAGE_FOOTER);
    dyad_writef(e->stream,  html);
}


static void onLine(dyad_Event *e) 
{
    char path[128];
    int trace_request_onlog = 1;

    if (sscanf(e->data, "GET %127s", path) == 1) 
    {
        
        // Send header
        dyad_writef(e->stream, "HTTP/1.1 200 OK\r\n\r\n");
        // Handle request
        if (!strcmp(path, "/"))      
        {
            handle_home_page(e);
        }
        else if (!strcmp(path, "/log"))   
        {
            handle_log_page(e);
        }
        else if ( strstr(path, "/log/multicon_") != NULL) 
        {
            handle_log_content_page(e, path);
            trace_request_onlog = 0;
        }
        else if (!strcmp(path, "/share")) 
        {
            handle_share_page(e); 
        }
        else if (!strcmp(path, "/settings")) 
        {
            handle_settings_page(e); 
        }
        else if (!strcmp(path, "/favicon.ico")) 
        {
            trace_request_onlog = 0;;
        }
        else 
        {
            system_logger(LOGGER_ERROR,"WEBSRV", "Requested  invalid path %s", path);
            dyad_writef(e->stream, "bad request '%s'", path);
        }


        // Print request
        if(!trace_request_onlog)
            system_logger(LOGGER_INFO,"WEBSRV", "Requested  %s %s", dyad_getAddress(e->stream), path);

        /* Close stream when all data has been sent */
        dyad_end(e->stream);
     }
}

static void onAccept(dyad_Event *e) {
  dyad_addListener(e->remote, DYAD_EVENT_LINE, onLine, NULL);
}

static void onListen(dyad_Event *e) {
    system_logger(LOGGER_INFO,"WEBSRV", "Server listening on %d", dyad_getPort(e->stream));
}

static void onError(dyad_Event *e) {
  system_logger(LOGGER_ERROR,"WEBSRV", "Server error %s", e->msg);

}


void http_server_run(){
    if (dyad_getStreamCount() > 0) {
    dyad_update();
  }
}

int http_server_init(){
  dyad_Stream *s;
  dyad_init();

  s = dyad_newStream();
  dyad_addListener(s, DYAD_EVENT_ERROR,  onError,  NULL);
  dyad_addListener(s, DYAD_EVENT_ACCEPT, onAccept, NULL);
  dyad_addListener(s, DYAD_EVENT_LISTEN, onListen, NULL);
  dyad_listen(s, 8000);
}

