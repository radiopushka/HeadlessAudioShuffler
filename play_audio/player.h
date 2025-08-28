#ifndef PLAYER
#define PLAYER

int setup_audio_interface(int channels, char* device, int rate);
void remove_audio_interface();

int play_song(char* path);

#endif // !PLAYER
