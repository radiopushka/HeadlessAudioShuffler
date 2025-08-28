#include "playlist.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <vorbis/vorbisfile.h>



char* file_last_prefix_(char* path,char ident){
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

void str_lower_(char* input){
  for(char* tr=input;*tr!=0;tr++){
      *tr=tolower(*tr);
  }
}

int is_valid_vorbis_(char* path){
  	OggVorbis_File vf;


      int err = ov_fopen(path, &vf);
   		if(err!=0){
   			return -1;
   		}

	ov_clear(&vf);
  return 1;
}

ID generate_playlist_(char* dir, int* entries){
  int num_entries = 0;
  ID songs=NULL;
  DIR* d= opendir(dir);
  if(!d)
    return NULL;

  struct dirent* dinf;


  while((dinf=readdir(d))!=NULL){
//determine if file
    if(dinf->d_type == FILE_T){
      int path_len = strlen(dir);
      int name_len = strlen(dinf->d_name);
      char* path = malloc(sizeof(char)*(path_len+name_len+2));
      sprintf(path,"%s/%s",dir,dinf->d_name);
      //determine if file is audio
      char* cut = file_last_prefix_(dinf->d_name,'.');
      str_lower_(cut);
      if(strcmp(cut,"ogg")==0 && (is_valid_vorbis_(path)==1)){
        //is audio file
        

        //handle play frequency settings
          char* name_copy = malloc(sizeof(char)*(name_len +1));
          memcpy(name_copy,dinf->d_name,sizeof(char)*(name_len+1));

          char* path_copy = malloc(sizeof(char)*(name_len+path_len +2));
          memcpy(path_copy,path,sizeof(char)*(name_len+path_len+2));
          num_entries++;
          ID song = malloc(sizeof(struct id_info));
          song->next = songs;
          song->path = path_copy;
          song->name = name_copy;
          songs = song;


      }
      free(path);
      free(cut);
    }

  }
  closedir(d);
  *entries = num_entries;
  return songs;
}
void free_song_list_(ID list){
  ID next;
  while(list != NULL){
    next = list->next;
    free(list -> name);
    free(list -> path);
    free(list);
    list = next;
  }
}
void free_song_list_blind_(ID list){
  ID next;
  while(list != NULL){
    next = list->next;
    free(list);
    list = next;
  }
}



//for quick retreival
int array_size_=0;
ID* song_array_=NULL;

ID get_next_id(){
  if(song_array_ == NULL)
    return NULL;
  int rsel =0;
  rsel=rand()%array_size_;
  return song_array_[rsel];
}


ID active_list_ = NULL;
void initialize_ids(char* dir){


  int entries;
  ID list =generate_playlist_(dir,&entries);
  
  if(list == NULL){
    active_list_ = NULL;
    song_array_ = NULL;
    return;
  }
  song_array_ = malloc(sizeof(struct song_info*)*entries);
  ID list_temp = list;
  ID* tmparr=song_array_;
  while(list_temp!=NULL){
    *tmparr = list_temp;
    tmparr++;
    list_temp = list_temp->next;
  }
  array_size_ = entries;
  active_list_=list;

}
void free_ids(){
  free(song_array_);
  free_song_list_(active_list_);

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
