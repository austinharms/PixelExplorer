#ifndef PACKAGE_H
#define PACKAGE_H

#include <unordered_map>
#include <string>

#include "subPackage/SubPackageLoader.h"
#include "subPackage/SubPackage.h"

class Package {
 public:
  static Package LoadPackage(std::string packageDirectory);
  static bool RegisterSubPackageLoader(SubPackageLoader* loader);
  static void UnregisterSubPackageLoaders();

 private:
  static std::unordered_map<uint16_t, SubPackageLoader*> s_subLoaders;

  std::unordered_map<uint16_t, SubPackage> _subPackages;
  Package();
  ~Package();
};

#endif  // !PACKAGELOADER_H
