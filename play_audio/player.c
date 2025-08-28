
#include <vorbis/vorbisfile.h>
#include <alsa/asoundlib.h>
#include "player.h"

snd_pcm_t *pcm_handle;


int BUFFER_SIZE=0;
snd_pcm_uframes_t frames;

int setup_audio_interface(int channels, char* device, int rate){

	
	if ( snd_pcm_open(&pcm_handle, device,SND_PCM_STREAM_PLAYBACK, 0) < 0){
		return -1;
	} 
	if(snd_pcm_set_params(pcm_handle, SND_PCM_FORMAT_S16_LE,SND_PCM_ACCESS_RW_INTERLEAVED, channels, rate, 1, 500000)<0){
		return -1;
	}
	frames=1024;
	BUFFER_SIZE = frames * channels * 2;

  return 1;
}

void remove_audio_interface(){

  snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
}

int write_buffer(char* buff, long frms){
  int pcm;

  if ((pcm = snd_pcm_writei(pcm_handle, buff, frms>>2)) == -EPIPE) {
			snd_pcm_prepare(pcm_handle);
		} else if (pcm < 0) {
			return -1;
		}
		return 1;

}
int play_song(char* path){

		OggVorbis_File vf;
  
	  char buffer[BUFFER_SIZE];
	  int err = ov_fopen(path, &vf);
   		if(err!=0){
   			return -1;
   		}
   
  long frms;
  int current_section;

	while((frms = ov_read(&vf, buffer, BUFFER_SIZE, 0, 2, 1,&current_section))!=0){

    write_buffer(buffer,frms);

  }
	ov_clear(&vf);
  return 1;


}
