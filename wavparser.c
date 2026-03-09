#include <unistd.h>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_ID_LEN 4

typedef struct HashMap {
  size_t size;
  size_t cap;
  void **buckets;
} HashMap;

const size_t BASE = 0x811c9dc5;

const size_t PRIME = 0x01000193;

size_t hash(HashMap *m, char *str) {
  size_t initial = BASE;
  while (*str) {
    initial ^= *str++;
    initial *= PRIME;
  }
  return initial & (m->cap - 1);
}

HashMap init(size_t cap) {
  HashMap m = {0, cap};
  m.buckets = malloc(sizeof(void *) * m.cap);
  assert(m.buckets != NULL);
  return m;
}

void insert(char *str, void *value, HashMap *m) {
  m->size++;
  m->buckets[hash(m, str)] = value;
}

void *get(HashMap *m, char *str) { return m->buckets[hash(m, str)]; }

typedef struct {
  char id[CHUNK_ID_LEN + 1];
  int size;
} WavChunk;

typedef struct {
  char id[CHUNK_ID_LEN + 1];
  int size;
  char info[100];
} InfoChunk;

typedef struct {
  int size;
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

ma_event shouldStop;
ma_format format;

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                   ma_uint32 frameCount) {
  ma_audio_buffer *pBuffer = pDevice->pUserData;
  if (pBuffer == NULL) {
    return;
  }
  int thing =
      ma_audio_buffer_read_pcm_frames(pBuffer, pOutput, frameCount, MA_FALSE);
  if (thing < frameCount) {
    ma_event_signal(&shouldStop);
  }
  (void)pInput;
}

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

InfoChunk read_info_chunk(FILE *fd) {
  InfoChunk infoChunk = {0};
  read_string(infoChunk.id, fd);
  fread(&infoChunk.size, 1, 4, fd);
  fread(infoChunk.info, sizeof(char), infoChunk.size, fd);
  return infoChunk;
}
WavChunk read_chunk(FILE *fd) {
  WavChunk wavChunk = {0};
  fread(wavChunk.id, sizeof(char), CHUNK_ID_LEN, fd);
  wavChunk.id[CHUNK_ID_LEN] = '\0';
  fread(&wavChunk.size, sizeof(wavChunk.size), 1, fd);
  return wavChunk;
}

AudioFormat read_audio_format(FILE *fd, int size) {
  AudioFormat audioFormat = {0};
  audioFormat.size = size;
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
  HashMap hm = init(500);
  int16_t *data;
  int data_size;
  // absolute cinema right here, yessir
  insert("AGES", "Rated", &hm);
  insert("CMNT", "Comment", &hm);
  insert("CODE", "EncodedBy", &hm);
  insert("COMM", "Comments", &hm);
  insert("DIRC", "Directory", &hm);
  insert("DISP", "SoundSchemeTitle", &hm);
  insert("DTIM", "DateTimeOriginal", &hm);
  insert("GENR", "Genre", &hm);
  insert("IARL", "ArchivalLocation", &hm);
  insert("IART", "Artist", &hm);
  insert("IAS1", "FirstLanguage", &hm);
  insert("IAS2", "SecondLanguage", &hm);
  insert("IAS3", "ThirdLanguage", &hm);
  insert("IAS4", "FourthLanguage", &hm);
  insert("IAS5", "FifthLanguage", &hm);
  insert("IAS6", "SixthLanguage", &hm);
  insert("IAS7", "SeventhLanguage", &hm);
  insert("IAS8", "EighthLanguage", &hm);
  insert("IAS9", "NinthLanguage", &hm);
  insert("IBSU", "BaseURL", &hm);
  insert("ICAS", "DefaultAudioStream", &hm);
  insert("ICDS", "CostumeDesigner", &hm);
  insert("ICMS", "Commissioned", &hm);
  insert("ICMT", "Comment", &hm);
  insert("ICNM", "Cinematographer", &hm);
  insert("ICNT", "Country", &hm);
  insert("ICOP", "Copyright", &hm);
  insert("ICRD", "DateCreated", &hm);
  insert("ICRP", "Cropped", &hm);
  insert("IDIM", "Dimensions", &hm);
  insert("IDIT", "DateTimeOriginal", &hm);
  insert("IDPI", "DotsPerInch", &hm);
  insert("IDST", "DistributedBy", &hm);
  insert("IEDT", "EditedBy", &hm);
  insert("IENC", "EncodedBy", &hm);
  insert("IENG", "Engineer", &hm);
  insert("IGNR", "Genre", &hm);
  insert("IKEY", "Keywords", &hm);
  insert("ILGT", "Lightness", &hm);
  insert("ILGU", "LogoURL", &hm);
  insert("ILIU", "LogoIconURL", &hm);
  insert("ILNG", "Language", &hm);
  insert("IMBI", "MoreInfoBannerImage", &hm);
  insert("IMBU", "MoreInfoBannerURL", &hm);
  insert("IMED", "Medium", &hm);
  insert("IMIT", "MoreInfoText", &hm);
  insert("IMIU", "MoreInfoURL", &hm);
  insert("IMUS", "MusicBy", &hm);
  insert("INAM", "Title", &hm);
  insert("IPDS", "ProductionDesigner", &hm);
  insert("IPLT", "NumColors", &hm);
  insert("IPRD", "Product", &hm);
  insert("IPRO", "ProducedBy", &hm);
  insert("IRIP", "RippedBy", &hm);
  insert("IRTD", "Rating", &hm);
  insert("ISBJ", "Subject", &hm);
  insert("ISFT", "Software", &hm);
  insert("ISGN", "SecondaryGenre", &hm);
  insert("ISHP", "Sharpness", &hm);
  insert("ISMP", "TimeCode", &hm);
  insert("ISRC", "Source", &hm);
  insert("ISRF", "SourceForm", &hm);
  insert("ISTD", "ProductionStudio", &hm);
  insert("ISTR", "Starring", &hm);
  insert("ITCH", "Technician", &hm);
  insert("ITRK", "TrackNumber", &hm);
  insert("IWMU", "WatermarkURL", &hm);
  insert("IWRI", "WrittenBy", &hm);
  insert("LANG", "Language", &hm);
  insert("LOCA", "Location", &hm);
  insert("PRT1", "Part", &hm);
  insert("PRT2", "NumberOfParts", &hm);
  insert("RATE", "Rate", &hm);
  insert("STAR", "Starring", &hm);
  insert("STAT", "Statistics", &hm);
  insert("TAPE", "TapeName", &hm);
  insert("TCDO", "EndTimecode", &hm);
  insert("TCOD", "StartTimecode", &hm);
  insert("TITL", "Title", &hm);
  insert("TLEN", "Length", &hm);
  insert("TORG", "Organization", &hm);
  insert("TRCK", "TrackNumber", &hm);
  insert("TURL", "URL", &hm);
  insert("TVER", "Version", &hm);
  insert("VMAJ", "VegasVersionMajor", &hm);
  insert("VMIN", "VegasVersionMinor", &hm);
  insert("YEAR", "Year", &hm);
  char *file = argv[1];
  if (argc < 2) {
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
  AudioFormat audioFmt = read_audio_format(fd, fmt.size);
  /* printf("Audio format details:\n  Audio Format: %d\n  Bits per Sample: "
         "%d\n  Sample Rate: %d Hz\n  Byte Rate: %d bytes/sec\n  Bitrate: %.2f "
         "Kbps\n "
         " Block Align: "
         "%d\n  Number of channels: %d\n",
         audioFmt.audio_format, audioFmt.bits_per_sample, audioFmt.sample_rate,
         audioFmt.byte_rate, ((double)audioFmt.byte_rate / 1000) * 8,
         audioFmt.block_align, audioFmt.num_channels); */
  WavChunk unknownChunk = read_chunk(fd);
  ChunkType type = get_chunk_type(&unknownChunk);
  switch (type) {
  case DATA_CHUNK:
    data_size = unknownChunk.size;
    int sample_count = unknownChunk.size * 8 / audioFmt.bits_per_sample;
    data = malloc(unknownChunk.size);
    int64_t things =
        fread(data, audioFmt.bits_per_sample / 8, sample_count, fd);
    break;
  case ID3_CHUNK:
    printf("ID3 Chunk");
    fseek(fd, unknownChunk.size, SEEK_CUR);
    break;
  case LIST_INFO_CHUNK:
    printf("INFO CHUNK");
    fseek(fd, 4,
          SEEK_CUR); // just the INFO tag name, not important
    InfoChunk chunk = read_info_chunk(fd);
    printf("ID: %s\nINFO: %s", chunk.id, chunk.info);
  }
  unknownChunk = read_chunk(fd);
  type = get_chunk_type(&unknownChunk);
  switch (type) {
  case DATA_CHUNK:
    printf("Data chunk found!!\n");
    int64_t *data = malloc(audioFmt.bits_per_sample / 8 * unknownChunk.size);
    fread(data, audioFmt.bits_per_sample / 8, unknownChunk.size, fd);
    printf("%d", data[0]);
    break;
  case ID3_CHUNK:
    printf("ID3 Chunk");
    break;
  case LIST_INFO_CHUNK:
    printf("INFO CHUNK\n");
    int bytes_read = 0;
    fseek(fd, 4,
          SEEK_CUR); // just the INFO tag name, not important
    bytes_read += 4;
    while (bytes_read < unknownChunk.size) {
      InfoChunk chunk = read_info_chunk(fd);
      printf("%s: %s\n", get(&hm, chunk.id), chunk.info);
      uint8_t byte;
      int is_empty = 1;
      fread(&byte, 1, 1, fd);
      if (byte != 0) {
        fseek(fd, -1, SEEK_CUR);
        is_empty = 0;
      }
      if (is_empty) {
        bytes_read += 4 + 4 + chunk.size + 1;
      } else {
        bytes_read += 4 + 4 + chunk.size;
      }
    }
  }
  unknownChunk = read_chunk(fd);
  type = get_chunk_type(&unknownChunk);
  switch (type) {
  case DATA_CHUNK:
    printf("Data chunk found!!\n");
    int64_t *data = malloc(audioFmt.bits_per_sample / 8 * unknownChunk.size);
    fread(data, audioFmt.bits_per_sample / 8, unknownChunk.size, fd);
    printf("%d", data[0]);
    break;
  case ID3_CHUNK:
    printf("ID3 Chunk\n");
    int bytes_read = 0;
    fseek(fd, 3, SEEK_CUR);
    uint8_t major;
    uint8_t minor;
    fread(&major, 1, 1, fd);
    fread(&minor, 1, 1, fd);
    fseek(fd, 5, SEEK_CUR); // we deal with this later (by later I mean never)
    bytes_read += 3 + 1 + 1 + 5; // look math is hard okay
    printf("IDv3 version is 2.%d.%d\n", major, minor);
    while (bytes_read < unknownChunk.size) {
      // bro i just wanna talk with whoever decided idv3 was a good idea
      char id[5];
      read_string(id, fd);
      fseek(fd, 3, SEEK_CUR);
      uint8_t size;
      fread(&size, 1, 1, fd);
      char info[size - 2]; // size of 13 -> only 10, size must be 11
      fseek(fd, 2, SEEK_CUR);
      fread(info, 1, size - 2, fd);
      info[size - 3] = '\0';
      fseek(fd, 1, SEEK_CUR);
      printf("%s %d %s", id, size, info);
      bytes_read += 4 + 3 + 1 + size;
    }
    break;
  case LIST_INFO_CHUNK:
    printf("INFO CHUNK\n");
    bytes_read = 0;
    fseek(fd, 4,
          SEEK_CUR); // just the INFO tag name, not important
    bytes_read += 4;
    while (bytes_read < unknownChunk.size) {
      InfoChunk chunk = read_info_chunk(fd);
      printf("%s: %s\n", get(&hm, chunk.id), chunk.info);
      uint8_t byte;
      int is_empty = 1;
      fread(&byte, 1, 1, fd);
      if (byte != 0) {
        fseek(fd, -1, SEEK_CUR);
        is_empty = 0;
      }
      if (is_empty) {
        bytes_read += 4 + 4 + chunk.size + 1;
      } else {
        bytes_read += 4 + 4 + chunk.size;
      }
    }
  }
  printf("\n");
  float duration = (float)data_size / audioFmt.byte_rate;
  // Audio playback

  ma_result result;
  ma_audio_buffer buff;
  ma_audio_buffer_config buffConfig;
  ma_device_config deviceConfig;
  ma_device device;
  switch (audioFmt.bits_per_sample) {
  case 16:
    format = ma_format_s16;
    break;
  case 24:
    format = ma_format_s24;
    break;
  }
  buffConfig = ma_audio_buffer_config_init(format, audioFmt.num_channels,
                                           data_size / audioFmt.num_channels /
                                               (audioFmt.bits_per_sample / 8),
                                           data, NULL);
  buffConfig.sampleRate = audioFmt.sample_rate;
  if (ma_audio_buffer_init(&buffConfig, &buff) != MA_SUCCESS) {
    return -1;
  }
  deviceConfig = ma_device_config_init(ma_device_type_playback);
  deviceConfig.playback.format = format;
  deviceConfig.playback.channels = audioFmt.num_channels;
  deviceConfig.sampleRate = audioFmt.sample_rate;
  deviceConfig.dataCallback = data_callback;
  deviceConfig.pUserData = &buff;
  if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
    return -1;
  }
  ma_event_init(&shouldStop);

  if (ma_device_start(&device) != MA_SUCCESS) {
    return -1;
  }
  int dur_seconds = (int)duration % 60;
  int dur_minutes = duration / 60;
  int dur_hours = dur_minutes / 60;
  dur_minutes = dur_minutes % 60;
  for (int i = 0; i <= (int)duration; i++) {
    int seconds = i % 60;
    int minutes = i / 60;
    int hours = minutes / 60;
    minutes = minutes % 60;
    printf("%02d:%02d:%02d / %02d:%02d:%02d (%d%%)", hours, minutes, seconds,
           dur_hours, dur_minutes, dur_seconds, (int)((i / duration) * 100));
    fflush(stdout);
    sleep(1);
    printf("\33[2K\r");
  }
  ma_event_wait(&shouldStop);
  ma_device_uninit(&device);
  ma_audio_buffer_uninit(&buff);
}
