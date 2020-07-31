//
//  read_tsv.hpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef file_reader_h
#define file_reader_h

#include <string>
#include <unordered_map>

#include "common.hpp"
#include "graph_auxiliary.hpp"

#ifdef BUILD_DLL
#define DLL_INTERFACE __declspec(dllexport)
#else
#define DLL_INTERFACE __declspec(dllimport)
#endif

#define REGISTER_PLUGIN(plugin_name, create_func) \
bool plugin_name ## _entry = bilab::FileReaderRegistry<Plugin>::add(#plugin_name, (create_func))

namespace bilab {
// Forward declaration
class FileReaderBase;

typedef FileReaderBase*(*readerInstanceGenerator)();

class FileReaderFactory
{
public:
  static FileReaderFactory& get();
  const char** getMenu(int& count);
  FileReaderBase* getFileReader(const char* typeName);
  bool registerGenerator(
                    const char* typeName,
                    const readerInstanceGenerator& funCreate);
private:
  FileReaderFactory();
  FileReaderFactory(const FileReaderFactory&);
  ~FileReaderFactory();
  
  std::unordered_map<std::string, readerInstanceGenerator> m_generators;
};

template<typename T>
class FileReaderRegistry {
public:
  typedef std::function<T*()> FactoryFunction;
  typedef std::unordered_map<std::string, FactoryFunction> FactoryMap;
  
  static bool add(const std::string& name, FactoryFunction fac) {
    auto map = getFactoryMap();
    //auto it = map.find(name);
    if ( map.find(name) != map.end()) {
       return false;
    }
    getFactoryMap()[name] = fac;
    return true;
  }
  
  static T* create(const std::string& name) {
    auto map = getFactoryMap();
    if (map.find(name) == map.end()) {
      return nullptr;
    }
    return map[name]();
  }
  
private:
  static FactoryMap& getFactoryMap() {
    static FactoryMap map;
    return map;
  }
};

class FileReaderBase {

public:
  virtual bool read(std::string& filename) const = 0;
};

template<typename T>
class FileReaderRegister {
public:
  FileReaderRegister(const char* id) {
    FileReaderFactory::get().registerGenerator (
       id,
       [](){ return static_cast<FileReaderBase*>(new T());}
    );
  }
};


} // End of namespace bilab

bool read_tsv(const std::string& tsv_file,
              bilab::DataFrame* df,
              std::string sep,
              std::string comment,
              int header);

#endif /* read_tsv_h */
