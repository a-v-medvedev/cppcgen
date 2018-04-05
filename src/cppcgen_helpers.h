/*
    Copyright (C) 2011-2013,2018 Alexey V. Medvedev

    This file is part of cppcgen. cppcgen is a generator of C code written in C++-11.

    cppcgen is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cppcgen is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cppcgen.  If not, see <http://www.gnu.org/licenses/>.
*/
 
#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdarg.h>
#include <string.h>

namespace cppcgen {

static inline std::string Mid(std::string &str, size_t a, size_t b) { return str.substr(a, b); }
static inline std::string Mid(std::string &str, size_t pos) { 
    if (pos < str.length()) return Mid(str, pos, str.length() - pos); 
    else return std::string(""); 
}

static inline void Insert(std::string &str, const std::string &s, size_t pos) { str.insert(pos, s); }

static inline void MakeLower(std::string &str) { (void)str; }

static inline std::string Left(std::string &str, size_t pos) { 
    if (pos == 0) return std::string(""); 
    if (pos >= str.length()) return str; return str.substr(str.length() - pos, pos); 
}

static inline void FormatV(std::string &str, const char *Format, va_list SourceArgList) {
  std::vector<char> buffer;
  buffer.resize(1024);
  ssize_t count;
  while ((count = vsnprintf(&buffer[0], 1024, Format, SourceArgList)) < 0 ||
         static_cast<size_t>(count) >= 1024)
    buffer.resize(buffer.size() + 1024);
  if (count) {
    buffer.resize(count);
    str.assign(&buffer[0], buffer.size());
  }
  else str.assign("");
}

static inline void Format(std::string &str, const char *Format, ...) {
  va_list argList;
  va_start(argList, Format);
  FormatV(str, Format, argList);
  va_end(argList);
}

static inline void Delete(std::string &str, size_t pos, size_t count) { 
    str.erase(str.begin() + pos, str.begin() + pos + count); 
}

static inline size_t Replace(std::string &str, const std::string &o, const std::string &n) { 
    size_t count = 0, pos = 0;
    while (true) {
        pos = str.find(o, pos);
        if (pos == std::string::npos)
            break;
        Delete(str, pos, o.length());
        Insert(str, n, pos);
        pos += n.length();
        count++;
    }
    return count;
}

static inline bool Trim(std::string &str) {
    size_t oldlen = str.length();
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), str.end());        
    return oldlen != str.length();
}
//    bool Trim(const char *chars) {
//        return Trim();
//    }
static inline ssize_t Find(std::string &str, char c) {
    size_t pos = str.find(c);
    if (pos == std::string::npos)
        return -1;
    return pos;
}

namespace Helpers {
/*
  // very basic fixed length string
  struct TString {
    char *str;
    static const size_t n;
    TString() : str(NULL) { 
      str = (char *)malloc(n); 
      if (str == 0) throw 0; 
    } 
    ~TString() { 
      free(str); 
    }
    void print(const char *fmt, ...) {
      va_list args; 
      va_start(args, fmt);
      _vsnprintf_s(str, n, n - 1, fmt, args);
      va_end(args);
      size_t len = strlen(str);
      if (len >= n - 1 && str[len - 1] != '\n') {
        str[n - 2] = '\n'; 
        str[n - 1] = 0;
      } else if (str[len - 1] != '\n') {
        str[len] = '\n';
        str[len + 1] = 0;
      }
    }
  };
*/

  // very basic output helper
  struct Output {
    FILE *fp;
    //Section cs;
    void open(const char *name) {
      fp = fopen(name, "w");
      if (fp == NULL)
          throw std::runtime_error("Helpers::Output: can't open file for writing");
    }
    void close() {
      if (fp != NULL)
        fclose(fp);
      fp = NULL;
    }
    Output() : fp(NULL) { }
    ~Output() { /*Locker lock(cs); */ close(); }
  };

  static inline size_t Split(std::string Str, char Delimiter, std::vector<std::string> &Arr) {
    Arr.clear();
    const char *s = Str.c_str();
    bool stop = false;
    do {
      const char *t = strchr(s, Delimiter);
      if (t == NULL) {
        stop = true;
        t = Str.c_str() + Str.length();
      }
      std::string item = Mid(Str, s - Str.c_str(), t - s);
      Arr.push_back(item);
      s = t + 1;
    } while (!stop);
    if (Arr.size() == 1 && Arr[0].empty())
      Arr.erase(Arr.begin());
    return Arr.size();
  }

  static inline void AddWordsToStr(std::string Add, std::string &Str) {
    size_t S = 0;
    size_t End = Str.length();
    while (End > S && Str[--End] == ' ');
    if (strchr(Add.c_str(), ' ') != NULL)
      throw std::runtime_error("Helpers::AddWordsToStr: bad input string");
    while (Str[S] == ' ') S++;
    if (Str[S] != 0) {
      Insert(Str, Add, S);
      S += Add.length();
      End += Add.length();
    }

    while (true) {
      const char *p = strchr(Str.c_str() + S, ' ');
      if (p == NULL)
          break;
      size_t pos = p - Str.c_str();
      if (pos > End)
        break;
      Insert(Str, Add, pos + 1);
      S = pos + Add.length() + 1;
      End += Add.length();
    }
  }

    static inline bool isvowel(char c) {
      return c == 'a' || c == 'o' || c == 'u' || c == 'i' || c == 'e' || c == 'y';
    }

    static inline void MakeAbbrev(std::string &Abbrev) {
      MakeLower(Abbrev);
      unsigned mode = 0;
      unsigned cons = 0;
      unsigned j = 0;
      for (; j < Abbrev.length(); j++) {
        if (mode == 0 && isvowel(Abbrev[j])) {
          mode = 1;
            continue;
        }
        if (mode == 0 && !isvowel(Abbrev[j])) {
          if (++cons == 2)
            break;
        }
        if (mode == 1 && !isvowel(Abbrev[j])) {
            mode = 2;
            break;
        }
      }
      Abbrev = Left(Abbrev, j + 1);
    }

}

}
