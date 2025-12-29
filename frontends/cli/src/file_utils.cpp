#include "file_utils.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

bool FileUtils::readLines(const std::string &filePath,
                          std::vector<std::string> &lines) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    return false;
  }

  lines.clear();
  std::string line;
  while (std::getline(file, line)) {
    lines.push_back(line);
  }

  file.close();
  return true;
}

bool FileUtils::writeLines(const std::string &filePath,
                           const std::vector<std::string> &lines) {
  std::ofstream file(filePath);
  if (!file.is_open()) {
    return false;
  }

  for (const auto &line : lines) {
    file << line << "\n";
  }

  file.close();
  return true;
}

bool FileUtils::fileExists(const std::string &filePath) {
  struct stat buffer;
  return (stat(filePath.c_str(), &buffer) == 0);
}

long FileUtils::getFileSize(const std::string &filePath) {
  struct stat buffer;
  if (stat(filePath.c_str(), &buffer) != 0) {
    return -1;
  }
  return buffer.st_size;
}
