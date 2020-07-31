//
//  reader_tsv.hpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef reader_tsv_hpp
#define reader_tsv_hpp

#include "graph_auxiliary.hpp"
#include "file_reader_factory.hpp"

class ReaderTSV: public bilab::FileReaderBase
{
public:
  virtual bool read(std::string& filename) const override {return true;};
  bool read(const std::string& tsv_file,
                bilab::DataFrame* df,
                std::string sep,
                std::string comment,
                int header);
};

namespace bilab {
  FileReaderRegister<ReaderTSV> _ReaderTSV("tsv");
}
#endif /* reader_tsv_hpp */
