#ifndef FILEUTILITIES_H
#define FILEUTILITIES_H

#include <string>

namespace px::util {
class FileUtilities {
 public:
  static bool directoryExists(const std::string path);
  static bool directoryExists(const char* path);
  static bool fileExists(const std::string path);
  static bool fileExists(const char* path);
  static const std::string getResourceDirectory();
  static const std::string getExecutingDirectory();
  static const std::string getAssetDirectory();

 private:
  static std::string _exeDir;
  static std::string _resDir;

  static std::string getExeDir();
  static std::string getResDir();
  FileUtilities() {}
  ~FileUtilities() {}
};
}  // namespace px::util
#endif  // !FILEUTILITIES_H
