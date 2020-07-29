//
//  print.hpp
//  networkz
//
//  Created by 曹巍 on 2020/07/25.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef print_h
#define print_h

//#include <functional>
#include <cstddef>
#include <streambuf>
//#include <ostream>
#include <iostream>

class teebuf : public std::streambuf
{
public:
  teebuf(std::streambuf* sb1, std::streambuf* sb2)
  :sb1(sb1), sb2(sb2)
  {}
  
private:
  virtual int overflow(int c) {
    if (c == EOF) {
      return !EOF;
    }else{
      const int r1 = sb1->sputc(c);
      const int r2 = sb2->sputc(c);
      return r1 == EOF || r2 == EOF ? EOF : c;
    }
  }
  
  virtual int sync() {
    const int r1 = sb1->pubsync();
    const int r2 = sb2->pubsync();
    return r1 == 0 && r2 == 0 ? 0 : -1;
  }
private:
  std::streambuf* sb1;
  std::streambuf* sb2;
};

class teestream : public std::ostream
{
public:
  // Construct an ostream which tees output to the supplied
  // ostreams.
  teestream(std::ostream& o1, std::ostream& o2)
  : std::ostream(&tbuf) , tbuf(o1.rdbuf(), o2.rdbuf())
  {}
  
private:
  teebuf tbuf;
};

class PrintBuf : public std::streambuf
{
public:
  PrintBuf(std::streambuf* sb)
  :sb(sb)
  {}

  void operator()(std::streambuf* _sb) {
    sb = _sb;
  }

private:
  virtual int overflow(int c) {
    if (c == EOF) {
      return !EOF;
    }else{
      const int r1 = sb->sputc(c);
      return r1 == EOF ? EOF : c;
    }
  }
  
  virtual int sync() {
    const int r1 = sb->pubsync();
    return r1 == 0 ? 0 : -1;
  }

private:
  std::streambuf* sb;
};

class Print : public std::ostream
{
public:
  Print()
  :std::ostream(&pbuf), pbuf(nullptr)
  {}
  
  Print(std::ostream& o)
  : std::ostream(&pbuf), pbuf(o.rdbuf())
  {}
  
  void operator()(std::ostream& o){
    pbuf(o.rdbuf());
    //std::ostream(&pbuf);
  }
private:
  PrintBuf pbuf;
};

#endif /* print_h */
