﻿#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>

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
    /// @brief translate string to JObject
    /// @param s , this is a 
    /// @return , the result
    static JObject* Parse(const std::string& fromStr);

    /// @brief find element in JObject
    /// @param s , element key
    /// @return , element value , if not found, nullptr
    JObject* Get(const std::string& s);

    int64_t ToInt() {
      return valueLL;
    }

    std::string ToString() {
      return valueString;
    }

    std::string ToJson();

    JsonValueType Type() {
      return valueType;
    }

    using Str = typename std::string;
    using StrItr = typename Str::iterator;

  private:

    template<typename K, typename V>
    using StdMap = typename std::map<K, V>;

    class Childs {
    private:
      StdMap<Str, JObject*> elements_;
    public:
      ~Childs();
      void Add(const Str& key, JObject* value);
      bool Remove(const Str& key);
      void Clear();
      JObject* Find(const Str& key){ 
        auto iter = elements_.find(key);
        return iter == elements_.end() ? nullptr : iter->second;
      }

      void InterToJson(std::vector<char>& vec);
    };


    /// @brief Constructor, but JObject is not initialized
    JObject();
    /// @brief Init Json {"key", "value"}
    /// @param iter , string.begin()
    /// @param iterEnd , string.end()
    /// @return ,if init success
    bool InitValue(StrItr iter, StrItr iterEnd);

    /// @brief 
    void UnInitValue();
    void SetInt(int64_t);
    bool SetInt(const StrItr&, const StrItr&);
    void SetStr(const StrItr&, const StrItr&);
    bool SplitKeyValues(StrItr, StrItr);

    void InterToJson(std::vector<char>& vec);


  private:
    JsonValueType valueType;
    int64_t valueLL;
    double valueDouble;
    bool valueBool; 
    Str valueString;
    Childs childs;
  };
}

