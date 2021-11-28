#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>

class Package {
 public:
  Package(std::string name, uint16_t version) { 
    _name = name;
    _path = "";
    _found = false;
    _version = version;
  }

  ~Package() {}

  void setPath(std::string path) const { 
    _found = true;
    _path = path;
  }

  std::string getName() const { return _name; }

  std::string getPath() const { return _path; }

  bool getPackageFound() const { return _found; }

  uint16_t getVersion() const { return _version; }

  bool operator<(const Package& rhs) const {
    return _name < rhs._name;
  }

 private:
  std::string _name;
  mutable std::string _path;
  mutable bool _found;
  uint16_t _version;
};
#endif