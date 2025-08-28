#ifndef PLAYLIST
#define PLAYLIST
#include "../config_file/config.h"

struct song_info{
  struct song_info* next;
  char* path;
  char* name;

};
typedef struct song_info* Song;

#define FILE_T 8

//clear the 3 stepped fifo, so that songs don't play after each other
//called once before any playlist initialization
void clear_fifo();
//free fifo memory before exiting the program
//called once before exiting the program
void free_fifo();


//the song dir is the directory where all the songs are
//only ogg files are accepted
//the config should have a section called 'songs' which will
//have a list of songs where it is 'song_name.ogg=3' so its frequency of playing will be 3
//config can be set to NULL
void initialize_pl(char* song_dir,CfgRaster config);

//this method returns the next song in the queue or NULL for end of queu
//this is a fake queu and the idea is that once this returns NULL, a refreshing of the playlist and configurations should occur
//bascially call initialize_pl and read the config files again once this returns NULL.
Song get_next();


//called after get_next returns NULL
void free_pl();

//FIFO manipulation
void put_fifo(Song song);

#endif // !PLAYLIST
