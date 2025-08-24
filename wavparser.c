#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_ID_LEN 4

typedef struct {
  char id[CHUNK_ID_LEN + 1];
  int size;
} WavChunk;

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
  printf("%s\n", riff.id);
  validate_string(riff.id, "RIFF", "No valid RIFF tag, exiting");
  char wave[5];
  read_string(wave, fd);
  printf("%s\n", wave);
  char fmt[5];
  read_string(fmt, fd);
  if (strcmp(fmt, "fmt ") != 0) {
    printf("This seems to be kinda sus, not a valid fmt header");
    exit(1);
  }
  printf("%s\n", fmt);
  fseek(fd, sizeof(char) * 4, SEEK_CUR);
  short audio_format;
  fread(&audio_format, sizeof(short), 1, fd);
  switch (audio_format) {
  case 1:
    printf("Is PCM wav file :3\n");
    break;
  default:
    printf("Some form of compression has been applied");
  }
  short num_channels;
  fread(&num_channels, sizeof(short), 1, fd);
  printf("The audio file has %hu channels\n", num_channels);
  int sample_rate;
  fread(&sample_rate, 4, 1, fd);
  printf("The audio file has a sample rate of %d Hz\n", sample_rate);
  fseek(fd, 4, SEEK_CUR); // Skip the byte rate, idk why you'd need this for
  short block_align;
  fread(&block_align, sizeof(short), 1, fd);
  printf("BlockAlign is: %hu\n", block_align);
  short bits_per_sample;
  fread(&bits_per_sample, sizeof(short), 1, fd);
  printf("The wav file has %hu bits per sample\n", bits_per_sample);
  char chunk_name[5];
  fread(chunk_name, sizeof(char), 4, fd);
  chunk_name[4] = '\0';
  printf("%s\n", chunk_name);
  if (strcmp(chunk_name, "data") != 0) {
    printf("Not a data chunk, TODO: parse info chunks");
    exit(1);
  }
  int chunk_size;
  fread(&chunk_size, 4, 1, fd);
  printf("chunk_size is %d bytes\n", chunk_size);
  printf("The length in seconds of the audio files is: %d seconds\n",
         (chunk_size * 8) / (bits_per_sample * sample_rate * num_channels));
  fseek(fd, chunk_size, SEEK_CUR);
  char info_chunk[5];
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
  fclose(fd);
}
