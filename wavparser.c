#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_ID_LEN 4

typedef struct {
  char id[CHUNK_ID_LEN + 1];
  int size;
} WavChunk;

typedef struct {
  char id[CHUNK_ID_LEN + 1];
  int size;
  char info[];
} InfoChunk;

typedef struct {
  short audio_format;
  short num_channels;
  int sample_rate;
  int byte_rate;
  short block_align;
  short bits_per_sample;
} AudioFormat;

typedef enum {
  DATA_CHUNK,
  ID3_CHUNK,
  LIST_INFO_CHUNK,
} ChunkType;

void validate_string(char *str1, char *str2, char *message) {
  if (strcmp(str1, str2) != 0) {
    printf("%s", message);
    exit(1);
  }
}

void read_string(char *buffer, FILE *fd) {
  fread(buffer, sizeof(char), 4, fd);
  buffer[CHUNK_ID_LEN] = '\0';
}

WavChunk read_chunk(FILE *fd) {
  WavChunk wavChunk = {0};
  fread(wavChunk.id, sizeof(char), CHUNK_ID_LEN, fd);
  wavChunk.id[CHUNK_ID_LEN] = '\0';
  fread(&wavChunk.size, sizeof(wavChunk.size), 1, fd);
  return wavChunk;
}

AudioFormat read_audio_format(FILE *fd) {
  AudioFormat audioFormat = {0};
  fread(&audioFormat.audio_format, sizeof(short), 1, fd);
  fread(&audioFormat.num_channels, sizeof(short), 1, fd);
  fread(&audioFormat.sample_rate, 4, 1, fd);
  fread(&audioFormat.byte_rate, 4, 1, fd);
  fread(&audioFormat.block_align, sizeof(short), 1, fd);
  fread(&audioFormat.bits_per_sample, sizeof(short), 1, fd);
  return audioFormat;
}

ChunkType get_chunk_type(WavChunk *chunk) {
  char *string = chunk->id;
  if (strcmp(string, "data") == 0) {
    return DATA_CHUNK;
  }
  if (strcmp(string, "id3 ") == 0 || strcmp(string, "ID3 ") == 0) {
    return ID3_CHUNK;
  }
  if (strcmp(string, "LIST") == 0) {
    return LIST_INFO_CHUNK;
  }
}

int main(int argc, char **argv) {
  char *file = argv[1];
  if (file == NULL) {
    printf("Please provide a wav file\n");
    exit(1);
  }
  printf("%s\n", file);

  FILE *fd = fopen(file, "r");
  if (!fd) {
    perror("File opening failed");
    exit(1);
  }
  WavChunk riff = read_chunk(fd);
  validate_string(riff.id, "RIFF", "No valid RIFF tag, exiting");
  char wave[5];
  read_string(wave, fd);
  validate_string(wave, "WAVE",
                  "This is a RIFF file but not a WAV file, exiting");
  WavChunk fmt = read_chunk(fd);
  validate_string(fmt.id, "fmt ", "Not a valid fmt header, exiting");
  AudioFormat audioFmt = read_audio_format(fd);
  printf("Audio format details:\n  Audio Format: %d\n  Bits per Sample: "
         "%d\n  Sample Rate: %d Hz\n  Byte Rate: %d bytes/sec\n  Block Align: "
         "%d\n  Number of channels: %d\n",
         audioFmt.audio_format, audioFmt.bits_per_sample, audioFmt.sample_rate,
         audioFmt.byte_rate, audioFmt.block_align, audioFmt.num_channels);
  WavChunk unknownChunk = read_chunk(fd);
  ChunkType type = get_chunk_type(&unknownChunk);
  switch (type) {
  case DATA_CHUNK:
    printf("Data chunk found!!\n");
    fseek(fd, unknownChunk.size, SEEK_CUR); // just skip over it for now;
    break;
  case ID3_CHUNK:
    printf("ID3 Chunk");
    break;
  case LIST_INFO_CHUNK:
    printf("INFO CHUNK");
    break;
  }
  printf("The length in seconds of the audio files is: %d seconds\n",
         (unknownChunk.size * 8) /
             (audioFmt.bits_per_sample * audioFmt.sample_rate *
              audioFmt.num_channels));
  /*char info_chunk[5];
  fread(info_chunk, sizeof(char), 4, fd);
  info_chunk[4] = '\0';
  printf("%s\n", info_chunk);
  int info_size;
  fread(&info_size, 4, 1, fd);
  printf("Info chunk size is %d\n", info_size);
  int str_length = info_size / sizeof(char);
  printf("%d", str_length);
  char meow[str_length + 1];
  fseek(fd, info_size, SEEK_CUR);
  printf("ID3 chunk, skipping for now");
  fclose(fd);*/
}
