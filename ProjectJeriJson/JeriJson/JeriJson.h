#pragma once

#include <string>
#include <map>

namespace JeriJson {
  
  enum class JsonValueType {
    ValueInt32,
    ValueInt64,
    ValueDouble,
    ValueBool,
    ValueString,
    ValueJson,
    ValueNull,
  };
  
  class JObject {
  public:
    static JObject* Parse(std::string& s);
    static JObject* Parse(const char* s, int maxLength);
    
    JObject* Get(std::string& s);
    JObject* Get(const char* s, int maxLength);

  private:
    bool Init(const char* s, int maxLength);
    JObject();

  private:
    JsonValueType valueType;
    int32_t valueInt;
    int64_t valueLL;
    double valueDouble;
    bool valueBool;
    char* valueString;
    std::map<std::string, JObject*> childs;
  };
}
