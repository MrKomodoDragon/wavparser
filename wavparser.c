#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
  char riff[5];
  fread(riff, sizeof(char), 4, fd);
  riff[4] = '\0';
  if (strcmp(riff, "RIFF") != 0) {
    printf("This is not a valid WAV file or not a WAV file, RIFF tag is not "
           "there");
    exit(1);
  }
  printf("%s\n", riff);
  fseek(fd, 4 * sizeof(char),
        SEEK_CUR); // 1 char is one byte, sanest way to ensure
                   //  that we infact do shift 4 bytes (please roast me if this
                   //  is wrong)
  char wave[5];
  fread(wave, sizeof(char), 4, fd);
  wave[4] = '\0';
  printf("%s\n", wave);
  char fmt[5];
  fread(fmt, sizeof(char), 4, fd); // fails somehow, check this later
  fmt[4] = '\0';
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
  fseek(fd, 4, SEEK_CUR); // Skip the byte rate, idk why you'd need this for now
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
}
