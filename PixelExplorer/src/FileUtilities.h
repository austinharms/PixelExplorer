#ifndef FILEUTILITIES_H
#define FILEUTILITIES_H
#include <string>

// Used for getting paths on windows
#include <Shlobj.h>
#include <windows.h>

#include <cassert>

class FileUtilities {
 public:
  static bool DirectoryExists(const std::string path) {
    return DirectoryExists(path.c_str());
  }

  static bool DirectoryExists(const char* path) {
    struct stat info;
    return stat(path, &info) == 0 && (info.st_mode & S_IFDIR);
  }

  static bool FileExists(const std::string path) {
    return FileExists(path.c_str());
  }

  static bool FileExists(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
  }

  static const std::string GetResourceDirectory() { return _resDir; }

  static const std::string GetExecutingDirectory() { return _exeDir; }

 private:
  static std::string _exeDir;
  static std::string _resDir;

  static std::string getExeDir() {
    TCHAR buffer[MAX_PATH] = {0};
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    std::wstring wpath = std::wstring(buffer).substr(0, pos);
    return std::string(wpath.begin(), wpath.end());
  };

  static std::string getResDir() {
    PWSTR path;
    auto get_folder_path_ret =
        SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path);
    if (get_folder_path_ret != S_OK) {
      CoTaskMemFree(path);
      assert(false);
      return "";
    }

    std::wstring wpath = std::wstring(path);
    CoTaskMemFree(path);
    return std::string(wpath.begin(), wpath.end());
  };

  FileUtilities() {}

  ~FileUtilities() {}
};

std::string FileUtilities::_exeDir = FileUtilities::getExeDir();
std::string FileUtilities::_resDir = FileUtilities::getResDir();
#endif  // !FILEUTILITIES_H
