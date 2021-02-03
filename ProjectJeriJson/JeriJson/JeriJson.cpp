#include "JeriJson.h"

JeriJson::JObject::JObject() :
  valueType(JsonValueType::ValueNull),
  valueInt(0),
  valueLL(0),
  valueDouble(0.0),
  valueBool(false),
  valueString(nullptr) {
}

inline JeriJson::JObject* JeriJson::JObject::Parse(std::string& s) {
  return Parse(s.c_str(), s.length());
}

inline JeriJson::JObject* JeriJson::JObject::Parse(const char* s, int maxLength) {
  JObject* object = new JObject();
  bool result = object->Init(s, maxLength);
  return result ? object : nullptr;
}

inline JeriJson::JObject* JeriJson::JObject::Get(std::string& s) {
  auto iter = childs.find(s);
  return iter != childs.end() ? iter->second : nullptr;
}

inline JeriJson::JObject* JeriJson::JObject::Get(const char* s, int maxLength) {
  return nullptr;
}

bool JeriJson::JObject::Init(const char* s, int maxLength) {
  return false;

}