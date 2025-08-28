#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char ptt[2];
char* str_append(char* before,char after){
  int bl=0;
  if(before != NULL){

    bl=strlen(before);
  }
  ptt[0]=after;
  ptt[1]=0;
  char* out=malloc((bl+2));
  memcpy(out,before,(bl));
  memcpy(out+bl,ptt,2);
  return out;
}
char* get_file_line(char* file){

  FILE* f= fopen(file,"r");
  if(!f)
    return NULL;

  char* first_line=NULL;
  
  char c;
  int hit=0;
  while((c=fgetc(f))!=EOF){
   if(c=='\n'){
      hit = 1;
      break;
    }else{
      char* novi = str_append(first_line,c);
      free(first_line);
      first_line=novi;
    }
  }
  if(hit ==0){
    fclose(f);
    return first_line;
  }
 
  int size = 0;
  char* remaining = NULL;

  while((c=fgetc(f))!=EOF){
      char* novi = str_append(remaining,c);
      free(remaining);
      remaining=novi;
      size++;
  }
  fclose(f);

  f=fopen(file,"w");
  if(!f){
    free(remaining);
    return first_line;
  }

  fwrite(remaining,sizeof(char),size,f);
  fclose(f);
  
  free(remaining);
  return first_line;

}

/*
int main(){
  char* item=get_file_line("./QUEUE");
  if(item==NULL){
    printf("null\n");
  }else{
    printf("%s\n",item);
  }
  free(item);
  return 0;

}*/
