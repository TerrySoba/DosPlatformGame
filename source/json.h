#ifndef JSON_H_INCLUDED
#define JSON_H_INCLUDED

#include "tiny_string.h"

// forward declare
struct cJSON;

class JsonValue
{
public:
    JsonValue(cJSON* data);

    bool isInvalid();
    bool isFalse();  
    bool isTrue();   
    bool isBool();   
    bool isNull();   
    bool isNumber(); 
    bool isString(); 
    bool isArray();  
    bool isObject(); 
    bool isRaw();

    int size();
    JsonValue at(int pos);
    JsonValue at(const char* name);

    TinyString keyString();
    TinyString toString();
    int toInt();
    double toDouble();
    
private:
    cJSON* m_data;
};

class Filename
{
public:
    Filename(const char* filename) : m_filename(filename) {}
    const char* getFilename() const { return m_filename.c_str(); }
private:
    TinyString m_filename;
};

class Json
{
public:
    Json(const char* data);
    Json(const Filename& filename);
    ~Json();

    JsonValue getRoot();
    
private:
    Json(const Json&);
    Json& operator=(const Json&);

private:
    cJSON* jsonRoot;
};

#endif