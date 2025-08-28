#include "../config_file/config.h"
#include "playlist.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <vorbis/vorbisfile.h>
#include <time.h>

//song fifo
 char* ten_list[3];
 int song_count=0;


char* file_last_prefix(char* path,char ident){
  char* name=malloc(sizeof(char)*256);
  memset(name,0,sizeof(char)*256);
  char* fslash=path;
  char* opath;
  int len=strlen(path)+1;
  if(len>255){
    len=255;
  }
  char* end=path+len;
  char* end2=end-1;


  for(opath=end2; opath>=path; opath--){
    if(*opath==ident && opath < end2){
      fslash=opath+1;
      break;
    }
  }

 memcpy(name,fslash,sizeof(char)*(end-fslash));
  name[255]=0;
  return name;
  
}

void str_lower(char* input){
  for(char* tr=input;*tr!=0;tr++){
      *tr=tolower(*tr);
  }
}

int is_valid_vorbis(char* path){
  	OggVorbis_File vf;


      int err = ov_fopen(path, &vf);
   		if(err!=0){
   			return -1;
   		}

	ov_clear(&vf);
  return 1;
}

  int n_diff_songs=0;
  char* diffcmp;
Song generate_playlist(CfgRaster config,char* dir, int* entries){
  int num_entries = 0;
  Song songs=NULL;
  DIR* d= opendir(dir);
  if(!d)
    return NULL;

  struct dirent* dinf;
  diffcmp=NULL;
  n_diff_songs=0;

  while((dinf=readdir(d))!=NULL){
//determine if file
    if(dinf->d_type == FILE_T){
      int path_len = strlen(dir);
      int name_len = strlen(dinf->d_name);
      char* path = malloc(sizeof(char)*(path_len+name_len+2));
      sprintf(path,"%s/%s",dir,dinf->d_name);
      //determine if file is audio
      char* cut = file_last_prefix(dinf->d_name,'.');
      str_lower(cut);
      if(strcmp(cut,"ogg")==0 && (is_valid_vorbis(path)==1)){
        //is audio file
        

        //handle play frequency settings
        char* frequency = get_value_by(config,"songs",dinf->d_name);
        int num_times = 1;
        if(frequency != NULL){
          num_times = atoi(frequency);
        }
        for(int i =0;i<num_times;i++){
          char* name_copy = malloc(sizeof(char)*(name_len +1));
          memcpy(name_copy,dinf->d_name,sizeof(char)*(name_len+1));

          char* path_copy = malloc(sizeof(char)*(name_len+path_len +2));
          memcpy(path_copy,path,sizeof(char)*(name_len+path_len+2));
          num_entries++;
          Song song = malloc(sizeof(struct song_info));
          song->next = songs;
          song->path = path_copy;
          song->name = name_copy;
          if(diffcmp==NULL || (strcmp(name_copy,diffcmp)!=0)){
            n_diff_songs++;
          }

          diffcmp = name_copy;
          songs = song;


        }
      }
      //printf("ndiff songs: %d\n",n_diff_songs);
      free(path);
      free(cut);
    }

  }
  closedir(d);
  *entries = num_entries;
  //clear FIFO, song counter
  song_count=0;
  return songs;
}
void free_fifo(){
  free(ten_list[0]);
  free(ten_list[1]);
  free(ten_list[2]);
}
void clear_fifo(){
  //init_rand();
  srand(time(NULL));
  ten_list[0]=NULL;
  ten_list[1]=NULL;
  ten_list[2]=NULL;

}
void free_song_list(Song list){
  Song next;
  while(list != NULL){
    next = list->next;
    free(list -> name);
    free(list -> path);
    free(list);
    list = next;
  }
}
void free_song_list_blind(Song list){
  Song next;
  while(list != NULL){
    next = list->next;
    free(list);
    list = next;
  }
}


int fifo_contains(char* name){
  
  for(int i=0;i<3;i++){
    char* pull = ten_list[i];
    if(pull != NULL){
      if(strcmp(pull,name)==0)
        return 1;
    }
  }
  return -1;
}

void put_fifo(Song song){
  int strl = strlen(song->name)+1;
  free(ten_list[2]);
  ten_list[2]=ten_list[1];
  ten_list[1]=ten_list[0];
  ten_list[0] = malloc(sizeof(char)*strl);
  memcpy(ten_list[0],song->name,strl*sizeof(char));
}

//for quick retreival
int array_size;
Song* song_array;

Song get_next(){
  if(song_array == NULL)
    return NULL;
  //to be able to detect end of the playlist and refresh
  if(song_count>array_size){
    return NULL;
  }
 // printf("ndiff songs: %d\n",n_diff_songs);
  song_count++;
  int rsel =0;
  for(int i=0;i<4000;i++){
      rsel=rand()%array_size;
      if(n_diff_songs<=3){
        put_fifo(song_array[rsel]);
        return song_array[rsel];

      }
      int is_fifo=fifo_contains(song_array[rsel]->name);
      if(is_fifo==-1){
        put_fifo(song_array[rsel]);
        return song_array[rsel];
      }
  }
  put_fifo(song_array[rsel]);
  return song_array[rsel];
}


Song active_list;
void initialize_pl(char* dir,CfgRaster config){

  int entries;
  Song list =generate_playlist(config,dir,&entries);
  
  if(list == NULL){
    active_list = NULL;
    song_array = NULL;
    return;
  }
  song_array = malloc(sizeof(struct song_info*)*entries);
  Song list_temp = list;
  Song* tmparr=song_array;
  while(list_temp!=NULL){
    *tmparr = list_temp;
    tmparr++;
    list_temp = list_temp->next;
  }
  array_size = entries;
  active_list=list;

}
void free_pl(){
  free(song_array);
  free_song_list(active_list);

}
/*
int main(){

  clear_fifo();
  initialize_pl("/home/ivan/radio",NULL);

  Song song;
  while((song = get_next())!=NULL){
    printf("%s\n",song->name);
  }
  free_pl();

  initialize_pl("/home/ivan/radio/",NULL);

  while((song = get_next())!=NULL){
    printf("%s\n",song->name);
  }
  free_pl();
  free_fifo();

}*/
