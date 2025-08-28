
all:
	gcc ./main.c ./playlist/playlist.c ./config_file/config.c ./queu/queu.c ./ids/playlist.c ./play_audio/player.c -lasound -lvorbis -lvorbisfile -Wall -g
