#pragma once

#include <string>
#include <map>
#include <functional>

namespace JeriJson {
  
  enum class JsonValueType {
    ValueNull,
    //ValueInt32,
    ValueInt64,
    //ValueDouble,
    //ValueBool,
    ValueString,
    ValueJson,
    ValueArray,
  };
  
  class JObject {
  public:
    static JObject* Parse(std::string& s);
    JObject* Get(std::string& s);

  private:
    using stdstr = typename std::string;
    using stritr = typename stdstr::iterator;
    template<typename K, typename V>
    using stdmap = typename std::map<K, V>;

    JObject();
    bool InitValue(stritr iter, stritr iterEnd);
    void UnInitValue();
    void SetInt(int64_t value);
    void SetStr(stritr iter, stritr iterEnd);
    void SetInt(stritr iter, stritr iterEnd);
    bool SplitKeyValues(stritr iter, stritr iterEnd);
    
    static bool TrimLeft(stritr& iterBegin, const stritr& iterEnd);
    static bool Trim(stritr& iter, stritr& iterEnd);
    static bool GetValue(stritr iter, stritr iterEnd, int64_t& value);

    static bool FindNextIterSkipSpace(stritr& iter, const stritr& iterEnd, const std::function<bool(char)>& match, stritr& iterFind);
    static bool FindNextIterSkipEscapesChar(stritr& iter, const stritr& iterEnd, const std::function<bool(char)>& match, stritr& iterFind);
    static bool FindBraceOrBracketPair(const stritr& iterBegin, const stritr& iterEnd, stritr& iterFind);
    static void FindNumberEnd(const stritr& iterBegin, const stritr& iterEnd, stritr& iterFind);

    static bool FindNextKeyRange(stritr& iter, const stritr& iterEnd, stritr& keyBegin, stritr& keyEnd);
    static bool FindNextKeyBegin(stritr& iter, const stritr& iterEnd, stritr& keyBegin);
    static bool FindNextKeyEnd(stritr& iter, const stritr& iterEnd, stritr& keyEnd);

    static bool FindNextColon(stritr& iter, const stritr& iterEnd);

    static bool FindNextValueRange(stritr& iter, const stritr& iterEnd, stritr& valueBegin, stritr& valueEnd);
    static bool FindNextValueBegin(stritr& iter, const stritr& iterEnd, stritr& valueBegin);
    static bool FindNextValueEnd(stritr& iter, const stritr& iterEnd, stritr& valueEnd);


    static bool IsSpaceChar(char c) {
      return c == 32;
    }

    static bool IsNumber(char c) {
      return c >= '0' && c <= '9';
    }

    static bool IsQuotationChar(char c) {
      return c == '"';
    }

    static bool IsEscapesChar(char c) {
      return c == '\\';
    }

    static bool IsColonChar(char c) {
      return c == ':';
    }

    static bool IsBraceChar(char c) {
      return c == '{';
    }

    static bool IsBracket(char c) {
      return c == '[';
    }

  private:
    static std::function<bool(char)> isQuotationFunc;
    static std::function<bool(char)> isColonFunc;
    const static int staticValue = 0;
  private:
    JsonValueType valueType;
    int64_t valueLL;
    double valueDouble;
    bool valueBool; 
    stdstr valueString;
    stdmap<stdstr, JObject*> childs;
  };
}

