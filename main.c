#include "./play_audio/player.h"
#include "./config_file/config.h"
#include "./playlist/playlist.h"
#include "./ids/playlist.h"
#include "./queu/queu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define FAKE

Song local_s;
char* queue=NULL;
int song_c=0;
int id_time=0;

Song local_next(){

  if(song_c>=id_time && id_time>0){
    ID ident=get_next_id();
    song_c=0;
    if(ident!=NULL){
      return (Song)ident;
    }
  }
  song_c++;

  free(queue);
  queue = get_file_line("QUEUE");
  if(queue == NULL){
    return get_next();
  }else{
    local_s->name=queue;
    local_s->path=queue;
    put_fifo(local_s);
    return local_s;

  }
}

int main(int argn , char* argv[]){

  if(argn<2){

    printf("specify a configuration file\n");
    return 0;
  }

  CfgRaster config = read_cfg_file(argv[1]);
  if(config == NULL){
    printf("configuration file invalid\n");
    return 0;
  }
  char* interface = get_value_by(config,"settings","device");

  char* rates = get_value_by(config,"settings","rate");
  int rate = atoi(rates);
  if(rate ==0){
    rate = 48000;
  }
  #ifndef FAKE
  
  if(interface == NULL){
    setup_audio_interface(2,"default",rate);
  }else{
    setup_audio_interface(2,interface,rate);
  }

  #endif /* ifndef FAKE */
  printf("rate: %d\ndevice: %s\n",rate,interface);
  clear_fifo();





  local_s=malloc(sizeof(struct song_info));
  int exit =0;
  while(exit ==0 ){//stop condition
    char* play_dir = get_value_by(config,"settings","root");

    if(play_dir == NULL)
      initialize_pl("./",config);
    else
      initialize_pl(play_dir,config);

    //initialize station ID logic
    char* id_dir = get_value_by(config,"settings","ids");
    int id_freq = 0;
    char *idfq = get_value_by(config,"settings","id_freq");
    if(idfq != NULL){
      id_freq = atoi(idfq);

    }
    free_ids();
    id_time=0;
    if(id_dir!=NULL && id_freq > 0){
        initialize_ids(id_dir);
        id_time=id_freq;
    }




    Song song;
    while((song = local_next())!=NULL){
      if(queue!=NULL && strcmp(queue,"exit")==0){
        exit =1;
        break;
      }
     //printf("now playing: %s\n",song->name);
  
      FILE* status = fopen("NOW PLAYING","w");
      if(status){
        fwrite(song->path,sizeof(char),strlen(song->path)+1,status);
        fclose(status);
      }
      #ifndef FAKE
      play_song(song->path);
      #else
       system("sleep 1");
      #endif
    }
    free_pl();

    CfgRaster config_D = read_cfg_file(argv[1]);
    if(config_D != NULL){
      free_cfg_mem(config);
      config = config_D;
    }

  }

  free_ids();
  free(local_s);
  free(queue);
  free_fifo();
  #ifndef FAKE
  remove_audio_interface();
  #endif
  free_cfg_mem(config);
  system("rm \"NOW PLAYING\"");
}
