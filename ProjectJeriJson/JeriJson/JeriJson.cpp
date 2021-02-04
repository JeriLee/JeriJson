#include "JeriJson.h"
#include <stack>

namespace JeriJson {
  JObject* JObject::Parse(std::string& s) {
    JObject* object = new JObject();
    bool result = object->InitValue(s.begin(), s.end());
    return result ? object : nullptr;
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

  void JObject::SetInt(int64_t value) {
    UnInitValue();
    valueType = JsonValueType::ValueInt32;
    valueLL = value;
  }

  void JObject::SetStr(stritr iter, stritr iterEnd) {
    UnInitValue();
    valueType = JsonValueType::ValueString;
    valueString = std::string(iter, iterEnd);
  }

  void JObject::SetInt(stritr iter, stritr iterEnd) {
    int64_t value;
    bool result = GetValue(iter, iterEnd, value);
    SetInt(value);
    //return result;
  }

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

  bool JObject::FindNextIterSkipEscapesChar(stritr& iter, const stritr& iterEnd, const std::function<bool(char)>& match, stritr& iterFind) {
    while (iter < iterEnd) {
      if (match(*iter)) {
        iterFind = iter++;
        return true;
      }
      if (IsEscapesChar(*iter)) {
        if (++iter == iterEnd) return false;
      }
      ++iter;
    }
    return false;
  }

  bool JObject::FindBraceOrBracketPair(const stritr& iterBegin, const stritr& iterEnd, stritr& iterFind) {
    iterFind = iterBegin;
    std::stack<bool> beginStack;
    beginStack.push(IsBraceChar(*iterFind++));
    stritr temp;

    while (iterFind < iterEnd) {
      switch (*iterFind) {
      case '"':
        if (!FindNextIterSkipEscapesChar(iterFind, iterEnd, isQuotationFunc, temp)) return false;
        break;
      case '{':
        beginStack.push(true);
        break;
      case '[':
        beginStack.push(false);
        break;
      case ']':
        if (beginStack.empty() || beginStack.top()) return false;
        beginStack.pop();
        if (beginStack.empty()) return true;
        break;
      case '}':
        if (beginStack.empty() || !beginStack.top()) return false;
        beginStack.pop();
        if (beginStack.empty()) return true;
        break;
      default:
        break;
      }
      ++iterFind;
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

  bool JObject::FindNextValueRange(stritr& iter, const stritr& iterEnd, stritr& valueBegin, stritr& valueEnd) {
    int allCount = 0;
    int quotationCount = 0;
    int trimCount = 0;

    if (TrimLeft(iter, iterEnd)) return false;
    // ""
    if (IsQuotationChar(*iter)) {
      valueBegin = iter;
      if (FindNextIterSkipEscapesChar(iter, iterEnd, isQuotationFunc, valueEnd)) return false;
      valueEnd = valueEnd;
      return true;
    }
    // 1234
    if (IsNumber(*iter)) {
      FindNumberEnd(iter, iterEnd, );
    }
    //{ }  [ ]
    if (IsBraceChar(*iter) || IsBracket(*iter)) {
      if (!FindBraceOrBracketPair(iter, iterEnd, valueEnd)) return false;
      valueBegin = iter;
      return true;
    }
    return false;
  }

  bool JObject::Trim(stritr& iter, stritr& iterEnd) {
    while (iter < iterEnd && IsSpaceChar(*iter)) {
      ++iter;
    }

    if (iter >= iterEnd) return false;

    while (IsSpaceChar(*(iterEnd - 1))) {
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
      childs.clear();
      break;
    default:
      break;
    }
    valueType = JsonValueType::ValueNull;
  }

  // Init j_son Value
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
    std::function<bool(char)> yinhao2 = std::function<bool(char)>([](char c)->bool { return c == '\"'; });
    while (true) {
      if (FindNextKeyRange(iter, iterEnd, keyBegin, keyEnd)) return false;
      if (FindNextColon(iter, iterEnd)) return false;
      if (FindNextValueRange(iter, iterEnd, valueBegin, valueEnd)) return false;
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
  //std::function<bool(char)> JObject::isQuotationFunc = [](char c)->bool { return c == '\"'; };
  std::function<bool(char)> JObject::isQuotationFunc = JObject::IsQuotationChar;
  std::function<bool(char)> JObject::isColonFunc = JObject::IsColonChar;
}