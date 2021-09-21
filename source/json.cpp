#include "json.h"
#include "exception.h"
#include "safe_file.h"

#include "3rd_party/cJSON/cJSON.h"

#include <stdio.h>
#include <malloc.h>

Json::Json(const char* data)
{
    if (!data)
    {
        throw Exception("Invalid data.");
    }
    jsonRoot = cJSON_Parse(data);
}

Json::Json(const Filename& filename)
{
    SafeFile fp(filename.getFilename(), "rb");

    fseek(fp.file(), 0, SEEK_END);
    size_t fileSize = ftell(fp.file());
    fseek(fp.file(), 0, SEEK_SET);
    char* buffer = (char*)malloc(fileSize + 1);
    fread(buffer, fileSize, 1, fp.file());

    jsonRoot = cJSON_Parse(buffer);
    free((void*)buffer);
}

Json::~Json()
{
    if (jsonRoot) cJSON_Delete(jsonRoot);
}

JsonValue Json::getRoot()
{
    return JsonValue(jsonRoot);
}

JsonValue::JsonValue(cJSON* data) :
    m_data(data)
{
}

int JsonValue::size()
{
    if (isArray() || isObject())
        return cJSON_GetArraySize(m_data);
    else
        return 0;
}

JsonValue JsonValue::at(int pos)
{
    cJSON* res = cJSON_GetArrayItem(m_data, pos);
    if (!res)
    {
        throw Exception("Index out of range.");
    }
    return JsonValue(res);
}

JsonValue JsonValue::at(const char* name)
{
    cJSON* res = cJSON_GetObjectItemCaseSensitive(m_data, name);
    if (!res)
    {
        throw Exception("Invalid object key.");
    }
    return JsonValue(res);
}

TinyString JsonValue::keyString()
{
    return m_data->string;
}

TinyString JsonValue::toString()
{
    if (!isString())
    {
        throw Exception("Not a string.");
    }
    return m_data->valuestring;
}

int JsonValue::toInt()
{
    if (!isNumber())
    {
        throw Exception("Not a number.");
    }
    return m_data->valueint;
}

double JsonValue::toDouble()
{
    if (!isNumber())
    {
        throw Exception("Not a number.");
    }
    return m_data->valuedouble;
}

bool JsonValue::isInvalid()
{
    return cJSON_IsInvalid(m_data);
}

bool JsonValue::isFalse()
{
    return cJSON_IsFalse(m_data);
}

bool JsonValue::isTrue()
{
    return cJSON_IsTrue(m_data);
}

bool JsonValue::isBool()
{
    return cJSON_IsBool(m_data);
}

bool JsonValue::isNull()
{
    return cJSON_IsNull(m_data);
}

bool JsonValue::isNumber()
{
    return cJSON_IsNumber(m_data);
}

bool JsonValue::isString()
{
    return cJSON_IsString(m_data);
}

bool JsonValue::isArray()
{
    return cJSON_IsArray(m_data);
}

bool JsonValue::isObject()
{
    return cJSON_IsObject(m_data);
}

bool JsonValue::isRaw()
{
    return cJSON_IsRaw(m_data);
}
