#include "JeriJson.h"

JeriJson::JObject::JObject() :
  valueType(JsonValueType::ValueNull),
  valueInt(0),
  valueLL(0),
  valueDouble(0.0),
  valueBool(false),
  valueString(nullptr) {
}

bool JeriJson::JObject::Init(const char* s, int maxLength) {
  return false;
}