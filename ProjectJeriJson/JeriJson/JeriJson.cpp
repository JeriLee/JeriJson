#include "JeriJson.h"

JeriJson::JObject* JeriJson::JObject::Parse(std::string& s) {
  JObject* object = new JObject();
  bool result = object->InitValue(s.begin(), s.end());
  return result ? object : nullptr;
}

JeriJson::JObject* JeriJson::JObject::Get(std::string& s) {
  auto iter = childs.find(s);
  return iter != childs.end() ? iter->second : nullptr;
}

JeriJson::JObject::JObject() :
  valueType(JsonValueType::ValueNull),
  valueLL(0L),
  valueDouble(0.0),
  valueBool(false),
  valueString(std::string()) {
}

void JeriJson::JObject::SetInt(int64_t value) {
  UnInitValue();
  valueType = JsonValueType::ValueInt32;
  valueLL = value;
}

void JeriJson::JObject::SetStr(stritr iter, stritr iterEnd) {
  UnInitValue();
  valueType = JsonValueType::ValueString;
  valueString = std::string(iter, iterEnd);
}

void JeriJson::JObject::SetInt(stritr iter, stritr iterEnd) {
  int64_t value;
  bool result = GetValue(iter, iterEnd, value);
  SetInt(value);
  //return result;
}

bool JeriJson::JObject::SplitKeyValues(stritr iterBegin, stritr iterEnd) {
  enum class StatusType {
    Default,
    KeyHalf,
    KeyOver,
    KeyFinish,
    ValueHalf,
    ValueOver,
    KvpSplit,
  };
  stritr iter = iterBegin;
  auto type = StatusType::Default;
  stritr keyBegin, keyEnd;
  stritr valueBegin, valueEnd;

  while (true) {
    Trim(iter, iterEnd);
    while (iter < iterEnd) {

    }



  }

  while (iter < iterEnd) {
    char c = *iter;
    if (c == '"') {
      switch (type) {
      case StatusType::Default:
      case StatusType::KvpSplit:
        type = StatusType::KeyHalf;
        keyBegin = iter;
        break;
      case StatusType::KeyHalf:
        type = StatusType::KeyOver;
        keyEnd = iter;
        break;
      case StatusType::KeyOver:
        type = StatusType::ValueHalf;
        valueBegin = iter;
        break;
      case StatusType::ValueHalf:
        type = StatusType::ValueOver;
        valueEnd = iter;
        break;
      case StatusType::ValueOver:
        //==
        break;
      }
    } else if (c == '\\') {
      ++iter;
    } else {

    }
    ++iter;
  }
}

bool JeriJson::JObject::GetValue(stritr iter, stritr iterEnd, int64_t& value) {
  value = 0;
  while (iter < iterEnd) {
    value = value * 10 + *iter++ - '0';
  }
  return true;
}

bool JeriJson::JObject::Trim(stritr iter, stritr iterEnd) {
  while (iter < iterEnd && IsSpaceChar(*iter)) {
    ++iter;
  }

  if (iter >= iterEnd) return false;

  while (IsSpaceChar(*(iterEnd - 1))) {
    --iterEnd;
  }
  return iter < iterEnd;
}

void JeriJson::JObject::UnInitValue() {
  switch (valueType) {
  case JeriJson::JsonValueType::ValueString:
    valueString = std::string();
    break;
  case JeriJson::JsonValueType::ValueJson:
    childs.clear();
    break;
  default:
    break;
  }
  valueType = JsonValueType::ValueNull;
}

// Init j_son Value
bool JeriJson::JObject::InitValue(stritr iter, stritr iterEnd) {
  if (!Trim(iter, iterEnd)) {
    return false;
  }

  char firstChar = *iter;
  if (iter + 1 == iterEnd) {
    if (IsNumber(firstChar)) {
      SetInt((int64_t)firstChar - '0');
      return true;
    }
    return false;
  }

  char lastChar = *(iterEnd - 1);
  if (firstChar == '{') {
    if (lastChar != '}') return false;
    SplitKeyValues(iter + 1, iterEnd - 1);
  } else if (firstChar == '\"') {
    if (lastChar != '\"') return false;
    SetStr(iter + 1, iterEnd - 1);
  } else if (IsNumber(firstChar)) {
    SetInt(iter, iterEnd);
  } else {
    //TO DO
    return false;
  }
  return true;
}






