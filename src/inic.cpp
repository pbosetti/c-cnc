/******************************************************************************\
 File:         inic.cpp
 Timestamp:    2020-Jun-01
 Author:       Paolo Bosetti <paolo.bosetti@unitn.it>
 Organization: Università di Trento - https://unitn.it/dii
 LICENSE:      All rights reserved (C) 2020
               Contains some MIT-licensed code as prior-knowledge, see the
               header of relevant source files
\******************************************************************************/
#include <fstream>
#include "inipp.h"
#include "inic.h"
using namespace std;

#define ini_get(t)                                                \
declare_ini_get(t) {                                              \
  t v = 0;                                                        \
  inipp::Ini<char> *ini = static_cast<inipp::Ini<char> *>(ini_p); \
  bool r = inipp::extract(ini->sections[section][field], v);      \
  *val = v;                                                       \
  return r ? 0 : 1;                                               \
}


void *ini_init(const char *path) {
  inipp::Ini<char> *ini = new inipp::Ini<char>();
  ifstream is(path);
  if (is.fail()) return NULL;
  ini->parse(is);
  ini->default_section(ini->sections["DEFAULT"]);
  ini->interpolate();
  if (ini->errors.empty())
    return ini;
  else
    return NULL;
}

void ini_free(void *ini) {
  delete static_cast<inipp::Ini<char> *>(ini);
}

ini_get(double);
ini_get(int);
ini_get(uint64_t);
ini_get(uint32_t);
ini_get(long);

int ini_get_char(void *ini_p, const char *section, const char *field, char *val, size_t len) {
  string str;
  inipp::Ini<char> *ini = static_cast<inipp::Ini<char> *>(ini_p);
  bool r = inipp::extract(ini->sections[section][field], str);
  strncpy(val, str.c_str(), len);
  return r ? 0 : 1;
}
