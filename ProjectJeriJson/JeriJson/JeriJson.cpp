#include "JeriJson.h"
#include <stack>

#include <iostream>

namespace JeriJson {
  inline bool IsSpaceChar(char c) {
    return c == 32;
  }

  inline bool IsNumber(char c) {
    return c >= '0' && c <= '9';
  }

  inline bool IsQuotationChar(char c) {
    return c == '"';
  }

  inline bool IsEscapesChar(char c) {
    return c == '\\';
  }

  inline bool IsColonChar(char c) {
    return c == ':';
  }

  inline bool IsBraceLChar(char c) {
    return c == '{';
  }

  inline bool IsBraceRChar(char c) {
    return c == '}';
  }

  inline bool IsBracketLChar(char c) {
    return c == '[';
  }

  inline bool IsBracketRChar(char c) {
    return c == ']';
  }

  bool Trim(JObject::StrItr& iter, JObject::StrItr& iterEnd, std::function<bool(char)>&& trimChar) {
    while (iter < iterEnd && trimChar(*iter)) {
      ++iter;
    }
    while (iter < iterEnd && trimChar(*(iterEnd - 1))) {
      --iterEnd;
    }
    return iter < iterEnd;
  }

  bool Trim(JObject::StrItr& iter, JObject::StrItr& iterEnd) {
    return Trim(iter, iterEnd, IsSpaceChar);
  }

  bool TrimLeft(JObject::StrItr& iter, const JObject::StrItr& iterEnd) {
    while (iter < iterEnd) {
      if (!IsSpaceChar(*iter)) return true;
      ++iter;
    }
    return false;
  }

  std::string TrimAllSpaceChar(const std::string& fromStr) {
    int len = fromStr.length(), cnt = 0;
    char* s = new char[len + 1];
    bool skip = true;
    for (auto c : fromStr) {
      if (IsQuotationChar(c)) skip = !skip;
      else if (skip && IsSpaceChar(c)) continue;
      s[cnt++] = c;
    }
    auto ret = std::string(s, s + cnt);
    delete[] s;
    return ret;
  }

  bool FindNextIterSkipSpace(const JObject::StrItr& iter, const JObject::StrItr& iterEnd, const std::function<bool(char)>& match, JObject::StrItr& iterFind) {
    iterFind = iter;
    while (iterFind < iterEnd) {
      if (match(*iterFind)) return true;
      if (!IsSpaceChar(*iterFind)) return false;
      ++iterFind;
    }
    return false;
  }

  void FindNumberEnd(const JObject::StrItr& iterBegin, const JObject::StrItr& iterEnd, JObject::StrItr& iterFind) {
    iterFind = iterBegin;
    while (iterFind < iterEnd && IsNumber(*iterFind)) {
      ++iterFind;
    }
  }

  bool FindNextIterSkipEscapesChar(const JObject::StrItr& iter, const JObject::StrItr& iterEnd, const std::function<bool(char)>& match, JObject::StrItr& iterFind) {
    iterFind = iter;
    while (iterFind < iterEnd) {
      if (match(*iterFind)) return true;
      if (IsEscapesChar(*iterFind)) ++iterFind;
      ++iterFind;
    }
    return false;
  }

  bool FindBraceOrBracketPair(const JObject::StrItr& iterBegin, const JObject::StrItr& iterEnd, JObject::StrItr& iterFind) {
    int count = 0;
    std::function<bool(char)> matchL = IsBraceLChar(*iterBegin) ? IsBraceLChar : IsBracketLChar;
    std::function<bool(char)> matchR = IsBraceLChar(*iterBegin) ? IsBraceRChar : IsBracketRChar;
    iterFind = iterBegin;
    while (iterFind < iterEnd) {
      count += matchL(*iterFind);
      count -= matchR(*iterFind);
      ++iterFind;
      if (count == 0) return true;
    }
    return false;
  }

  inline bool FindNextKeyBegin(const JObject::StrItr& iter, const JObject::StrItr& iterEnd, JObject::StrItr& keyBegin) {
    return FindNextIterSkipSpace(iter, iterEnd, IsQuotationChar, keyBegin);
  }

  inline bool FindNextKeyEnd(const JObject::StrItr& iter, const JObject::StrItr& iterEnd, JObject::StrItr& keyEnd) {
    bool result = FindNextIterSkipEscapesChar(iter, iterEnd, IsQuotationChar, keyEnd);
    keyEnd++;
    return result;
  }

  inline bool FindNextColon(const JObject::StrItr& iter, const JObject::StrItr& iterEnd, JObject::StrItr& iterFind) {
    return FindNextIterSkipSpace(iter, iterEnd, IsColonChar, iterFind);
  }

  inline bool FindNextKeyRange(const JObject::StrItr& iter, const JObject::StrItr& iterEnd, JObject::StrItr& keyBegin, JObject::StrItr& keyEnd) {
    if (!FindNextKeyBegin(iter, iterEnd, keyBegin)) return false;
    if (!FindNextKeyEnd(keyBegin + 1, iterEnd, keyEnd)) return false;
    return true;
  }

  // [valueBegin, valueEnd)
  bool FindNextValueRange(const JObject::StrItr& iterBegin, const JObject::StrItr& iterEnd, JObject::StrItr& valueBegin, JObject::StrItr& valueEnd) {
    if (IsQuotationChar(*iterBegin)) {
      if (!FindNextIterSkipEscapesChar(iterBegin + 1, iterEnd, IsQuotationChar, valueEnd)) return false;
      valueBegin = iterBegin;
      valueEnd++;
      return true;
    }

    if (IsNumber(*iterBegin)) {
      FindNumberEnd(iterBegin, iterEnd, valueEnd);
      valueBegin = iterBegin;
      return true;
    }

    if (IsBraceLChar(*iterBegin) || IsBracketLChar(*iterBegin)) {
      if (!FindBraceOrBracketPair(iterBegin, iterEnd, valueEnd)) return false;
      valueBegin = iterBegin;
      return true;
    }
    return false;
  }



  //=================
  JObject::Childs::~Childs() {
    Clear();
  }

  void JObject::Childs::Add(const Str& key, JObject* value) {
    auto iter = elements_.find(key);
    if (iter != elements_.end()) {
      delete iter->second;
    }
    elements_[key] = value;
  }

  bool JObject::Childs::Remove(const Str& key) {
    auto iter = elements_.find(key);
    if (iter != elements_.end()) {
      delete iter->second;
      elements_.erase(iter);
      return true;
    }
    return false;
  }

  void JObject::Childs::Clear() {
    for (const auto& iter : elements_) {
      delete iter.second;
    }
    elements_.clear();
  }

  void VecPushBackString(std::vector<char>& vec, const std::string& str) {
    vec.emplace_back('"');
    for (const char& c : str) vec.emplace_back(c);
    vec.emplace_back('"');
  }

  void JObject::Childs::InterToJson(std::vector<char>& vec) {
    bool first = true;
    for (auto& iter : elements_) {
      if (!first) {
        vec.emplace_back(',');
        first = false;
      }

      VecPushBackString(vec, iter.first);
      vec.emplace_back(':');
      iter.second->InterToJson(vec);
    }
  }

  //============
  JObject* JObject::Parse(const std::string& fromStr) {
    std::string jsonStr = TrimAllSpaceChar(fromStr);
    if (jsonStr.length() == 0) return nullptr;
    StrItr iterBegin, iterEnd;
    if (FindNextValueRange(jsonStr.begin(), jsonStr.end(), iterBegin, iterEnd) == false || iterBegin != jsonStr.begin() || iterEnd != jsonStr.end()) return nullptr;

    JObject* object = new JObject();
    bool result = object->InitValue(iterBegin, iterEnd);
    if (!result) {
      delete object;
      object = nullptr;
    }
    return object;
  }

  JObject* JObject::Get(const std::string& s) {
    return childs.Find(s);
  }

  std::string JObject::ToJson() {
    std::vector<char> vec;
    InterToJson(vec);
    return std::string(vec.begin(), vec.end());
  }

  void VecPushBackIntDig(std::vector<char>& vec, int64_t x) {
    if (x == 0) return;
    VecPushBackIntDig(vec, x / 10);
    vec.push_back(x % 10 + 48);
  }

  void VecPushBackInt(std::vector<char>& vec, int64_t x) {
    if (x == 0) vec.push_back(0);
    else {
      if (x < 0) {
        vec.push_back('-');
        x = -x;
      }
      VecPushBackIntDig(vec, x);
    }
  }

  void JObject::InterToJson(std::vector<char>& vec) {
    switch (valueType) {
    case JsonValueType::ValueInt64:
      VecPushBackInt(vec, valueLL);
      break;
    case JsonValueType::ValueString:
      VecPushBackString(vec, valueString);
      break;
    case JsonValueType::ValueJson:
      vec.push_back('{');
      childs.InterToJson(vec);
      vec.push_back('}');
    default:
      break;
    }
  }

  JObject::JObject() :
    valueType(JsonValueType::ValueNull),
    valueLL(0L),
    valueDouble(0.0),
    valueBool(false),
    valueString(std::string()) {
  }

  void JObject::UnInitValue() {
    switch (valueType) {
    case JeriJson::JsonValueType::ValueString:
      valueString = std::string();
      break;
    case JeriJson::JsonValueType::ValueJson:
      childs.Clear();
      break;
    default:
      break;
    }
    valueType = JsonValueType::ValueNull;
  }

  void JObject::SetInt(int64_t value) {
    UnInitValue();
    valueType = JsonValueType::ValueInt64;
    valueLL = value;
  }

  void JObject::SetStr(const StrItr& strBegin, const StrItr& strEnd) {
    UnInitValue();
    valueType = JsonValueType::ValueString;
    valueString = std::string(strBegin, strEnd);
  }

  bool GetValue(JObject::StrItr iter, const JObject::StrItr& iterEnd, int64_t& value) {
    value = 0;
    while (iter < iterEnd) {
      value = value * 10 + *iter++ - '0';
    }
    return true;
  }

  bool JObject::SetInt(const StrItr& iter, const StrItr& iterEnd) {
    //test lamda function
    int64_t value;
    bool result = GetValue(iter, iterEnd, value);
    if (result) SetInt(value);
    return result;
  }

  //From [ range )
  bool JObject::InitValue(StrItr iterBegin, StrItr iterEnd) {
    if (IsBraceLChar(*iterBegin)) {
      return SplitKeyValues(iterBegin + 1, iterEnd - 1);
    } else if (IsQuotationChar(*iterBegin)) {
      SetStr(iterBegin + 1, iterEnd - 1);
      return true;
    } else if (IsNumber(*iterBegin)) {
      SetInt(iterBegin, iterEnd);
      return true;
    } else if(IsBracketLChar(*iterBegin)){
      //TO DO
      return false;
    } else {
      //TO DO
      return false;
    }
    return false;
  }

  //From [iterBegin, iterEnd)
  bool JObject::SplitKeyValues(StrItr iterBegin, StrItr iterEnd) {
    StrItr iter = iterBegin, temp = iterBegin;
    StrItr keyBegin, keyEnd, valueBegin, valueEnd;

    while (true) {
      // from [iterBegin, iterEnd)
      // find key range [keyBegin, keyEnd)
      if (!FindNextKeyRange(iter, iterEnd, keyBegin, keyEnd)) return false;
      if (!FindNextColon(keyEnd, iterEnd, temp)) return false;
      // from [iter + 1, iterEnd)
      // find value range [valueBegin, valueEnd)
      iter = temp + 1;
      if (!FindNextValueRange(iter, iterEnd, valueBegin, valueEnd)) return false;

      JObject* value = new JObject();
      if (!(value->InitValue(valueBegin, valueEnd))) return false;
      childs.Add(std::string(keyBegin + 1, keyEnd - 1), value);

      iter = valueEnd;
      bool result = FindNextIterSkipSpace(iter, iterEnd, [](char c)->bool { return c == ','; }, temp);
      if (!result) {
        if (temp == iterEnd) break;
        else return false;
      }
      iter = temp + 1;
    }
    valueType = JsonValueType::ValueJson;
  }
}