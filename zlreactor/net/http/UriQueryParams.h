// ***********************************************************************
// Filename         : UriQueryParams.h
// Author           : LIZHENG
// Description      : 
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_URIQUERYPARAMS_H
#define ZL_URIQUERYPARAMS_H
#include <string>
#include <vector>

namespace zl
{
    namespace net
    {
        struct UriQueryParam
        {
        public:
            UriQueryParam() {}
            UriQueryParam(const std::string& name, const std::string& value)
                : name_(name)
                , value_(value)
            {
            }

            void name(const std::string& n) { name_ = n; }
            void value(const std::string& v) { value_ = v; }
            const std::string name() const  { return name_; }
            const std::string value() const { return value_; }

        public:
            std::string name_;
            std::string value_;
        };

        class UriQueryParams
        {
        public:
            bool parse(const std::string& params);
            void appendToString(std::string* target) const;
            void writeToString(std::string* target) const;
            std::string toString() const;

            const UriQueryParam* find(const std::string& name) const;
            UriQueryParam* find(const std::string& name);

            UriQueryParam& get(size_t index);
            const UriQueryParam& get(size_t index) const;

            bool getValue(const std::string& name, std::string* value) const;
            bool getValue(const std::string& name, int* value) const;
            const std::string& getOrDefaultValue(const std::string& name, const std::string& default_value) const;

            size_t count() const;
            void clear();
            bool isEmpty() const;
            void add(const UriQueryParam& param);
            void add(const std::string& name, const std::string& value);
            void set(const std::string& name, const std::string& value);
            bool remove(const std::string& name);

        private:
            std::vector<UriQueryParam> params_;
        };
    }
}
#endif  /* ZL_URIQUERYPARAMS_H */

