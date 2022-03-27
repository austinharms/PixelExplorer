#include "FileUtilities.h"

#include <filesystem>
// Used for getting paths on windows
#include <Shlobj.h>
#include <windows.h>

#include "Logger.h"

namespace px::util {
std::string FileUtilities::_exeDir = FileUtilities::getExeDir();
std::string FileUtilities::_resDir = FileUtilities::getResDir();

bool FileUtilities::directoryExists(const std::string path) {
  return directoryExists(path.c_str());
}

bool FileUtilities::directoryExists(const char* path) {
  struct stat info;
  return stat(path, &info) == 0 && (info.st_mode & S_IFDIR);
}

bool FileUtilities::fileExists(const std::string path) {
  return fileExists(path.c_str());
}

bool FileUtilities::fileExists(const char* path) {
  struct stat buffer;
  return (stat(path, &buffer) == 0);
}

const std::string FileUtilities::getResourceDirectory() { return _resDir; }

const std::string FileUtilities::getExecutingDirectory() { return _exeDir; }

const std::string FileUtilities::getAssetDirectory() {
  return _exeDir + "assets\\";
}

std::string FileUtilities::getExeDir() {
  TCHAR buffer[MAX_PATH] = {0};
  GetModuleFileName(NULL, buffer, MAX_PATH);
  std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
  std::wstring wpath = std::wstring(buffer).substr(0, pos);
  std::string fullPath = std::string(wpath.begin(), wpath.end()) + "\\";
  return fullPath;
};

std::string FileUtilities::getResDir() {
  PWSTR path;
  auto get_folder_path_ret =
      SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path);
  if (get_folder_path_ret != S_OK) {
    CoTaskMemFree(path);
    Logger::fatal("Failed to get Resource directory");
    return "";
  }

  std::wstring wpath = std::wstring(path);
  CoTaskMemFree(path);
  std::string fullPath =
      std::string(wpath.begin(), wpath.end()) + "\\PX\\0.0.0\\";
  if (!directoryExists(fullPath)) {
    if (!std::filesystem::create_directories(fullPath)) {
      Logger::warn("Failed to create Resource Directory");
    }
  }

  return fullPath;
};
}  // namespace px::util