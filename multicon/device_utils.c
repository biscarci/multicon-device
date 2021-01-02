#include "device_utils.h"




char* util_strcpy(char* dest, const char* source, int dest_size)
{
    int i ;

    if(source)
    {
        for(i=0; i < dest_size-1 && source[i] !='\0'; i++)
        {
            dest[i] = source[i];
        }
        dest[i] = '\0';
    }
    else
    {
        dest[i] = '\0';
    }
}

void util_snprintf(char* dest, int destSize, const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);

    vsnprintf(dest, destSize, fmt, args);

    dest[destSize-1] = '\0';
    va_end(args);
}



/* Esempio di Json inviato dal server ed interpolato dal multicon sulla falsa riga 

{
  "id": 1,
  "method": "call",
  "token": "590bde71578da2fabfe77ba86c00e4e5",
  "params": [
    "uci",
    "get",
    "wireless.@wifi-iface[0].ssid"
  ]
}

{\"id\": 1, \"method\": \"call\", \"token\": \"chiave-di-autenticazione\", \"params\": [ \"uci\", \"get\", \"wireless.@wifi-iface[0].ssid\"] }




int main(int argc, char** argv)
{
  char* file_contents = "{\"id\": 1, \"method\": \"call\", \"token\": \"chiave-di-autenticazione\", \"params\": [ \"uci\", \"get\", \"wireless.@wifi-iface[0].ssid\"] }";
  json_char* json;
  json_value* value;

  json = (json_char*)file_contents;

  printf("Start test main for json char\n");
  
  value = json_parse(json, strlen(file_contents));


  json_parse_command(value);

  json_value_free(value);


  return 0;
}
*/
