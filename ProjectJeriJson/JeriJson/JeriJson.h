#pragma once

/*
union JeriJsonValue {
  int value;
}
*/

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
    static JObject* Parse(std::string& s) {
      return Parse(s.c_str(), s.length());
    }

    static JObject* Parse(const char* s, int maxLength) {
      JObject* object = new JObject();
      bool result = object->Init(s, maxLength);
      return result ? object : nullptr;
    }
    
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
    std::map<std::string, JObject> childs;
  };
}
