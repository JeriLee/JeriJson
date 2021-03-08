#pragma once

#include <string>
#include <map>
#include <functional>

namespace JeriJson {

  class ClassA {
    int value = 0;
  private:
    static bool Function(int x, int y) {
      //该函数不访问成员变量
    }
  };

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
    /// @brief translate string to JObject
    /// @param s , the string
    /// @return , the result
    static JObject* Parse(std::string& s);

    /// @brief find element in JObject
    /// @param s , element key
    /// @return , element value , if not found, nullptr
    JObject* Get(std::string& s);

    using Str = typename std::string;
    using StrItr = typename Str::iterator;

  private:

    template<typename K, typename V>
    using stdmap = typename std::map<K, V>;

    class Childs {
    private:
      stdmap<Str, JObject*> keyToValue_;
    public:
      ~Childs();
      void Add(const Str& key, JObject* value);
      bool Remove(const Str& key);
    };


    /// @brief Constructor, but JObject is not initialized
    JObject();
    ~JObject();
    /// @brief Init Json {"key", "value"}
    /// @param iter , string.begin()
    /// @param iterEnd , string.end()
    /// @return ,if init success
    bool InitValue(StrItr iter, StrItr iterEnd);

    /// @brief 
    void UnInitValue();
    void SetInt(int64_t value);
    void SetStr(StrItr iter, StrItr iterEnd);
    bool SetInt(StrItr iter, StrItr iterEnd);
    bool SplitKeyValues(StrItr iter, StrItr iterEnd);
    
    static bool GetValue(StrItr iter, StrItr iterEnd, int64_t& value);

  private:
    JsonValueType valueType;
    int64_t valueLL;
    double valueDouble;
    bool valueBool; 
    Str valueString;
    stdmap<Str, JObject*> childs;
  };
}

