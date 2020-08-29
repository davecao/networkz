//
//  plugin_factory.hpp
//  networkz
//
//  Created by CAO Wei on 2020/07/31.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef FILE_READER_FACTORY_H
#define FILE_READER_FACTORY_H

#include <iostream>
#include <map>

#include "FileReaderBase.hpp"

/// Macro for class registration
#define REGISTER_FACTORY(derivedClass) \
namespace { \
auto registry_ ## derivedClass = \
  NARO::ConcreteFactory<derivedClass>(#derivedClass); \
}

namespace NARO {
/**
 * @brief Factory class for file reader
 *
 * It uses std::map to automatically register derived reader classes.
 */
class FileReaderFactory{

private:
    using FactoryMap = std::map<std::string, FileReaderFactory*>;
    // Force global variable to be initialized, thus it avoid
    // the inialization order fisaco.
    static auto getRegister() -> FactoryMap& {
       static FactoryMap classRegister{};
       return classRegister;
    }

public:
    /**
     * Register factory object of derived class
     */
    static auto registerFactory(
                const std::string& name,
                FileReaderFactory* factory) -> void {
         auto& reg = FileReaderFactory::getRegister();
         reg[name] = factory;
    }
    /**
     * Show all registered classes
     */
    static auto showClasses() -> void {
         std::cout << " Registered classes. " << "\n";
         std::cout << " =================== " << "\n";
         for(const auto& pair: FileReaderFactory::getRegister())
                 std::cout << " + " << pair.first << "\n";
    }
    /**
     * Construct derived class returning a raw pointer
     */
    static auto makeRaw(const std::string& name) -> FileReaderBase* {
         auto it = FileReaderFactory::getRegister().find(name);
         if(it != FileReaderFactory::getRegister().end())
                 return it->second->construct();
         return nullptr;
    }

    /**
     * Construct derived class returning an unique ptr
     */
    static auto makeUnique(const std::string& name) -> std::unique_ptr<FileReaderBase>{
        return std::unique_ptr<FileReaderBase>(FileReaderFactory::makeRaw(name));
    }

    /**
     * Destructor
     * the virtual function
     */
    virtual ~FileReaderFactory() = default;
    /**
     * @fn construct
     * A pure virtual function
     */
    virtual auto construct() const -> FileReaderBase* = 0;
};
/**
 * The concrete factory class to register the derived class
 * @tparam DerivedClass the user defined class
 */
template<typename DerivedClass>
class ConcreteFactory: FileReaderFactory { 
public:
   /**
    * Constructor
    *
    * Register this global object on the Factory register
    */
   ConcreteFactory(const std::string& name){
      //std::cerr << " [TRACE] " << " Registered Class = " << name << "\n";
      FileReaderFactory::registerFactory(name, this);
   }
  /**
   * @fn construct
   * The implementation of the pure virtual function defined in the \link FileReaderFactory \endlink.
   */
   auto construct() const -> DerivedClass* {
      return new DerivedClass;
   }
};

} // End of namespace NARO

#endif /* FILE_READER_FACTORY_H */
