#pragma once

#include <string>
#include <map>

namespace JeriJson {
  
  enum class JsonValueType {
    ValueNull,
    ValueInt32,
    ValueInt64,
    ValueDouble,
    ValueBool,
    ValueString,
    ValueJson,
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
    
    static bool Trim(stritr iter, stritr iterEnd);
    static bool GetValue(stritr iter, stritr iterEnd, int64_t& value);
    static bool IsSpaceChar(char c) {
      return c == 32;
    }
    static bool IsNumber(char c) {
      return c >= '0' && c <= '9';
    }

  private:
    JsonValueType valueType;
    int64_t valueLL;
    double valueDouble;
    bool valueBool;
    stdstr valueString;
    stdmap<stdstr, JObject*> childs;
  };
}
