#ifndef IDS
#define IDS

struct id_info{
  struct id_info* next;
  char* path;
  char* name;

};
typedef struct id_info* ID;

#define FILE_T 8



void initialize_ids(char* dir);

ID get_next_id();


void free_ids();


#endif // !PLAYLIST
