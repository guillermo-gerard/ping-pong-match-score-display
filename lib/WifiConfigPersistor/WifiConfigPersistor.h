#ifndef _WifiConfigPersistor_H
#define _WifiConfigPersistor_H

#include <Config.h>

class WifiConfigPersistor 
{
private:
    const char *_filename;
public:
    WifiConfigPersistor(const char *filename) { _filename = filename; }
    Config Retrieve();
    bool Persist(Config config);
};

#endif