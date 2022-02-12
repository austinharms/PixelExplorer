#include "PackageLoader.h"

#include <filesystem>
#include <fstream>

#include "FileUtilities.h"
#include "Logger.h"

std::unordered_map<uint16_t, PackageModuleLoader*>
    PackageLoader::s_moduleLoaders;

Package* PackageLoader::LoadPackageByName(std::string packageName) {
  if (FileUtilities::DirectoryExists(FileUtilities::GetExecutingDirectory() +
                                     "packages")) {
    for (auto& p : std::filesystem::directory_iterator(
             FileUtilities::GetExecutingDirectory() + "packages")) {
      if (p.is_directory()) {
        Package* pkg = ScanPackage(p.path().generic_u8string());
        if (pkg != nullptr) {
          if (pkg->_name == packageName) return LoadPackage(pkg);
          pkg->drop();
        }
      }
    }
  }

  if (FileUtilities::DirectoryExists(FileUtilities::GetResourceDirectory() +
                                     "packages")) {
    for (auto& p : std::filesystem::directory_iterator(
             FileUtilities::GetResourceDirectory() + "packages")) {
      if (p.is_directory()) {
        Package* pkg = ScanPackage(p.path().generic_u8string());
        if (pkg != nullptr) {
          if (pkg->_name == packageName) return LoadPackage(pkg);
          pkg->drop();
        }
      }
    }
  }

  Logger::Warn("Failed to Find Package Width Name: " + packageName);
  Logger::Debug("Scanned Directories:\n\t\"" + FileUtilities::GetResourceDirectory() +
                "packages\"\n\t\"" + FileUtilities::GetExecutingDirectory() +
                "packages\"");

  return nullptr;
}

Package* PackageLoader::LoadPackage(std::string packageDirectory) {
  return LoadPackage(ScanPackage(packageDirectory));
}

Package* PackageLoader::LoadPackage(Package* pkg) {
  if (pkg == nullptr) return nullptr;

  for (auto& modLoader : s_moduleLoaders)
    pkg->AddModule(modLoader.second->Load(pkg->_path));

  pkg->_modulesLoaded = true;
  Logger::Info("Loaded Package: " + pkg->_name + ", Loaded " +
               std::to_string(pkg->_modules.size()) + " Modules");
  return pkg;
}

Package* PackageLoader::ScanPackage(std::string packageDirectory) {
  if (FileUtilities::FileExists(packageDirectory + "/package.properties")) {
    Package* pkg = new Package();
    pkg->_path = packageDirectory;
    std::ifstream packageFile(
        (packageDirectory + "/package.properties").c_str(), std::ios::binary);
    if (packageFile &&
        packageFile.peek() != std::ifstream::traits_type::eof()) {
      while (packageFile.peek() != std::ifstream::traits_type::eof()) {
        pkg->_name += packageFile.get();
      }

      packageFile.close();
      pkg->_propertiesLoaded = true;
      return pkg;
    }

    packageFile.close();
    pkg->drop();
  }

  Logger::Debug("Failed to Find Valid package.properties File");
  return nullptr;
}

Package* PackageLoader::ScanPackages(std::string packagesDirectory,
                                     uint16_t& packageCount) {
  return nullptr;
}
