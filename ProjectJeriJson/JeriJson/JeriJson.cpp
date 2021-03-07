#include "JeriJson.h"
#include <stack>

namespace JeriJson {
  JObject* JObject::Parse(std::string& s) {
    JObject* object = new JObject();
    bool result = object->InitValue(s.begin(), s.end());
    if (!result) {
      delete object;
      object = nullptr;
    }
    return object;
  }

  JObject* JObject::Get(std::string& s) {
    auto iter = childs.find(s);
    return iter != childs.end() ? iter->second : nullptr;
  }

  JObject::JObject() :
    valueType(JsonValueType::ValueNull),
    valueLL(0L),
    valueDouble(0.0),
    valueBool(false),
    valueString(std::string()) {
  }

  JObject::~JObject() {
    for (auto object : childs) {
      delete object.second;
    }
  }

  void JObject::SetInt(int64_t value) {
    UnInitValue();
    valueType = JsonValueType::ValueInt64;
    valueLL = value;
  }

  void JObject::SetStr(stritr iter, stritr iterEnd) {
    UnInitValue();
    valueType = JsonValueType::ValueString;
    valueString = std::string(iter, iterEnd);
  }

  //TO DO
  bool JObject::SetInt(stritr iter, stritr iterEnd) {
    int64_t value;
    bool result = GetValue(iter, iterEnd, value);
    if (result) SetInt(value);
    return result;
  }

  //TO DO
  bool JObject::GetValue(stritr iter, stritr iterEnd, int64_t& value) {
    value = 0;
    while (iter < iterEnd) {
      value = value * 10 + *iter++ - '0';
    }
    return true;
  }

  bool JObject::FindNextIterSkipSpace(stritr& iter, const stritr& iterEnd, const std::function<bool(char)>& match, stritr& iterFind) {
    while (iter < iterEnd) {
      if (match(*iter)) {
        iterFind = iter++;
        return true;
      }
      if (!IsSpaceChar(*iter)) return false;
      ++iter;
    }
    return false;
  }

  bool JObject::FindNextIterSkipEscapesChar(const stritr& iter, const stritr& iterEnd, const std::function<bool(char)>& match, stritr& iterFind) {
    iterFind = iter;
    while (iterFind < iterEnd) {
      if (match(*iterFind)) {
        return true;
      }
      if (IsEscapesChar(*iterFind)) {
        ++iterFind;
      }
      ++iterFind;
    }
    return false;
  }

  bool JObject::FindBraceOrBracketPair(const stritr& iterBegin, const stritr& iterEnd, stritr& iterFind) {
    int count = 0;

    char leftChar = *iterBegin;
    char endChar = leftChar == '{' ? '}' : ']';

    while (iterFind < iterEnd) {
      count += (*iterFind == leftChar);
      count -= (*iterFind == endChar);
      if (count == 0) {
        return true;
      }
    }
    return false;
  }

  void JObject::FindNumberEnd(const stritr& iterBegin, const stritr& iterEnd, stritr& iterFind) {
    iterFind = iterBegin;
    while (iterFind < iterEnd && IsNumber(*iterFind)) {
      ++iterFind;
    }
  }

  bool JObject::FindNextKeyBegin(stritr& iter, const stritr& iterEnd, stritr& keyBegin) {
    return FindNextIterSkipSpace(iter, iterEnd, isQuotationFunc, keyBegin);
  }

  bool JObject::FindNextKeyEnd(stritr& iter, const stritr& iterEnd, stritr& keyEnd) {
    return FindNextIterSkipEscapesChar(iter, iterEnd, isQuotationFunc, keyEnd);
  }

  bool JObject::FindNextColon(stritr& iter, const stritr& iterEnd) {
    stritr iterFind;
    return FindNextIterSkipSpace(iter, iterEnd, isColonFunc, iterFind);
  }

  bool JObject::FindNextValueBegin(stritr& iter, const stritr& iterEnd, stritr& valueBegin) {
    return FindNextIterSkipSpace(iter, iterEnd, isQuotationFunc, valueBegin);
  }

  bool JObject::FindNextValueEnd(stritr& iter, const stritr& iterEnd, stritr& valueEnd) {
    return false;
  }

  bool JObject::FindNextKeyRange(stritr& iter, const stritr& iterEnd, stritr& keyBegin, stritr& keyEnd) {
    if (!FindNextKeyBegin(iter, iterEnd, keyBegin)) return false;
    if (!FindNextKeyEnd(iter, iterEnd, keyEnd)) return false;
    return true;
  }

  //[iter, iterEnd) FindValueRange
  bool JObject::FindNextValueRange(stritr& iter, const stritr& iterEnd, stritr& valueBegin, stritr& valueEnd) {
    if (!TrimLeft(iter, iterEnd)) return false;

    // ""
    if (IsQuotationChar(*iter)) {
      valueBegin = iter;
      return FindNextIterSkipEscapesChar(iter, iterEnd, isQuotationFunc, valueEnd);
    }

    // 1234
    if (IsNumber(*iter)) {
      valueBegin = iter;
      FindNumberEnd(iter, iterEnd, valueEnd);
      iter = valueEnd;
    }

    //{ }  [ ]
    if (IsBraceChar(*iter) || IsBracket(*iter)) {
      valueBegin = iter;
      return FindBraceOrBracketPair(iter, iterEnd, valueEnd);
    }
    return false;
  }

  bool JObject::Trim(stritr& iter, stritr& iterEnd) {
    return Trim(iter, iterEnd, IsSpaceChar);
  }

  bool JObject::Trim(stritr& iter, stritr& iterEnd, std::function<bool(char)>&& trimChar) {
    while (iter < iterEnd && trimChar(*iter)) {
      ++iter;
    }
    while (iter < iterEnd && trimChar(*(iterEnd - 1))) {
      --iterEnd;
    }
    return iter < iterEnd;
  }

  bool JObject::TrimLeft(stritr& iter, const stritr& iterEnd) {
    while (iter < iterEnd) {
      if (!IsSpaceChar(*iter)) return true;
      ++iter;
    }
    return false;
  }

  void JObject::UnInitValue() {
    switch (valueType) {
    case JeriJson::JsonValueType::ValueString:
      valueString = std::string();
      break;
    case JeriJson::JsonValueType::ValueJson:
      for (auto object : childs) {
        delete object.second;
      }
      childs.clear();
      break;
    default:
      break;
    }
    valueType = JsonValueType::ValueNull;
  }

  bool JObject::InitValue(stritr iter, stritr iterEnd) {
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
      if (lastChar == '}') {
        return SplitKeyValues(iter + 1, iterEnd - 1);
      } else {
        return false;
      }
    } else if (IsQuotationChar(firstChar)) {
      stritr iterFind;
      bool result = FindNextIterSkipEscapesChar(iter + 1, iterEnd, IsQuotationChar, iterFind);
      if (result && iterFind + 1 == iterEnd) {
        SetStr(iter + 1, iterEnd - 1);
        return true;
      } else {
        return false;
      }
    } else if (IsNumber(firstChar)) {
      SetInt(iter, iterEnd);
    } else if(firstChar == '['){
      //TO DO
      return false;
    } else {
      return false;
    }
    return true;
  }

  bool JObject::SplitKeyValues(stritr iterBegin, stritr iterEnd) {
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
      if (!FindNextKeyRange(iter, iterEnd, keyBegin, keyEnd)) return false;
      if (!FindNextColon(iter, iterEnd)) return false;
      if (!FindNextValueRange(iter, iterEnd, valueBegin, valueEnd)) return false;

      JObject* value = new JObject();
      if (!(value->InitValue(valueBegin, valueEnd))) return false;
      childs.emplace(std::string(keyBegin, keyEnd), value);
      stritr _;
      bool result = FindNextIterSkipSpace(iter, iterEnd, [](char c)->bool { return c == ','; }, _);
      if (!result) {
        //if()
      }
    }
  }
  //std::function<bool(char)> JObject::isQuotationFunc = [](char c)->bool { return c == '\"'; };
  std::function<bool(char)> JObject::isQuotationFunc = JObject::IsQuotationChar;
  std::function<bool(char)> JObject::isColonFunc = JObject::IsColonChar;
}