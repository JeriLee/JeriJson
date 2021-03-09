#include "JeriJson.h"
#include <stack>

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

  void TrimAllSpaceChar(std::string& formatStr) {
    int len = formatStr.length();
    char* s = new char[len + 1];
    int cnt = 0;
    bool skip = true;
    for (auto c : formatStr) {
      if (IsQuotationChar(c)) skip = !skip;
      else if (skip && IsSpaceChar(c)) continue;
      
      s[cnt++] = c;
    }
    formatStr = std::string(s, s + cnt);
    delete[] s;
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
    JObject::StrItr iter = iterBegin;
    if (!TrimLeft(iter, iterEnd)) return false;

    if (IsQuotationChar(*iter)) {
      if (!FindNextIterSkipEscapesChar(iter, iterEnd, IsQuotationChar, valueEnd)) return false;
      valueBegin = iter;
      valueEnd++;
      return true;
    }

    if (IsNumber(*iter)) {
      FindNumberEnd(iter, iterEnd, valueEnd);
      valueBegin = iter;
      return true;
    }

    if (IsBraceLChar(*iter) || IsBracketLChar(*iter)) {
      if (!FindBraceOrBracketPair(iter, iterEnd, valueEnd)) return false;
      valueBegin = iter;
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

  //============
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

  //Check
  JObject::JObject() :
    valueType(JsonValueType::ValueNull),
    valueLL(0L),
    valueDouble(0.0),
    valueBool(false),
    valueString(std::string()) {
  }
  //Check
  JObject::~JObject() {
    for (auto object : childs) {
      delete object.second;
    }
  }
  //Check
  void JObject::SetInt(int64_t value) {
    UnInitValue();
    valueType = JsonValueType::ValueInt64;
    valueLL = value;
  }
  //Check
  void JObject::SetStr(stritr iter, stritr iterEnd) {
    UnInitValue();
    valueType = JsonValueType::ValueString;
    valueString = std::string(iter, iterEnd);
  }
  //Check
  //TO DO
  bool JObject::SetInt(stritr iter, stritr iterEnd) {
    int64_t value;
    bool result = GetValue(iter, iterEnd, value);
    if (result) SetInt(value);
    return result;
  }
  //Check
  //TO DO
  bool JObject::GetValue(stritr iter, stritr iterEnd, int64_t& value) {
    value = 0;
    while (iter < iterEnd) {
      value = value * 10 + *iter++ - '0';
    }
    return true;
  }








  //Check
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

  bool JObject::InitValue(StrItr iterBegin, StrItr iterEnd) {
    if (!Trim(iterBegin, iterEnd)) {
      return false;
    }

    if (IsBraceLChar(*iterBegin)) {
      if (IsBraceRChar(*(iterEnd - 1))) {
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

  //From [iterBegin, iterEnd)
  bool JObject::SplitKeyValues(StrItr iterBegin, StrItr iterEnd) {
    StrItr iter;
    StrItr keyBegin, keyEnd, valueBegin, valueEnd;

    while (true) {
      // from [iterBegin, iterEnd)
      // find key range [keyBegin, keyEnd)
      if (!FindNextKeyRange(iterBegin, iterEnd, keyBegin, keyEnd)) return false;
      if (!FindNextColon(keyEnd, iterEnd, iter)) return false;
      // from [iter + 1, iterEnd)
      // find value range [valueBegin, valueEnd)
      if (!FindNextValueRange(iter + 1, iterEnd, valueBegin, valueEnd)) return false;

      JObject* value = new JObject();
      if (!(value->InitValue(valueBegin, valueEnd))) return false;
      childs.Add(std::string(keyBegin, keyEnd), value);
      stritr _;
      bool result = FindNextIterSkipSpace(iter, iterEnd, [](char c)->bool { return c == ','; }, _);
      if (!result) {
        //if()
      }
    }
  }
  //std::function<bool(char)> JObject::isQuotationFunc = [](char c)->bool { return c == '\"'; };
  //std::function<bool(char)> JObject::isQuotationFunc = JObject::IsQuotationChar;
  //std::function<bool(char)> JObject::isColonFunc = JObject::IsColonChar;
}