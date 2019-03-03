#define _CRT_SECURE_NO_WARNINGS
#include "io.h"
#include <cstdio>


bool ReadAllBytes(std::string fileName, std::vector<uint8_t> * output) {
  output->clear();
  std::FILE * file = std::fopen(fileName.c_str(), "rb");
  if (!file) {
    return false;
  }

  uint8_t buffer[1024];
  
  while (!std::feof(file)) {
    size_t numBytesRead = std::fread(buffer, sizeof(uint8_t), 1024, file);
    
    size_t prevVecSize = output->size();
    output->resize(prevVecSize + numBytesRead);
    std::memcpy(output->data() + prevVecSize, buffer, numBytesRead);
  }
  std::fclose(file);

  return true;
}