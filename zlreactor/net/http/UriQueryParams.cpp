#include "zlreactor/net/http/UriQueryParams.h"
#include "zlreactor/base/StringUtil.h"
#include <algorithm>

namespace zl
{
    namespace net
    {
        bool UriQueryParams::parse(const std::string& params)
        {
            clear();
            std::vector<std::string> splited;
            zl::base::split(params, splited, "&");
            for (size_t i = 0; i < splited.size(); ++i)
            {
                params_.push_back(UriQueryParam());
                size_t pos = splited[i].find('=');
                if (pos != std::string::npos)
                {
                    params_.back().name_.assign(splited[i], 0, pos);
                    params_.back().value_.assign(splited[i], pos + 1, std::string::npos);
                }
                else
                {
                    params_.back().name_ = splited[i];
                }
            }
            return true;
        }

        void UriQueryParams::appendToString(std::string* target) const
        {
            for (size_t i = 0; i < params_.size(); ++i)
            {
                if (!params_[i].name_.empty())
                {
                    const UriQueryParam& param = params_[i];
                    target->append(param.name_);
                    target->push_back('=');
                    target->append(param.value_);
                    if (i != params_.size() - 1)
                        target->push_back('&');
                }
            }
        }

        void UriQueryParams::writeToString(std::string* target) const
        {
            target->clear();
            appendToString(target);
        }

        std::string UriQueryParams::toString() const
        {
            std::string result;
            appendToString(&result);
            return result;
        }

        UriQueryParam* UriQueryParams::find(const std::string& name)
        {
            for (size_t i = 0; i < params_.size(); ++i)
            {
                if (params_[i].name_ == name)
                    return &params_[i];
            }
            return NULL;
        }

        const UriQueryParam* UriQueryParams::find(const std::string& name) const
        {
            return const_cast<UriQueryParams*>(this)->find(name);
        }

        UriQueryParam& UriQueryParams::get(size_t index)
        {
            return params_.at(index);
        }

        const UriQueryParam& UriQueryParams::get(size_t index) const
        {
            return params_.at(index);
        }

        bool UriQueryParams::getValue(const std::string& name, std::string* value) const
        {
            const UriQueryParam* param = find(name);
            if (param)
            {
                *value = param->value_;
                return true;
            }
            return false;
        }

        bool UriQueryParams::getValue(const std::string& name, int* value) const
        {
            const UriQueryParam* param = find(name);
            if (param)
            {
                *value = atoi(param->value_.c_str());
                return true;
            }
            return false;
        }

        const std::string& UriQueryParams::getOrDefaultValue(const std::string& name, const std::string& default_value) const
        {
            const UriQueryParam* param = find(name);
            if (param)
            {
                return param->value_;
            }
            return default_value;
        }

        size_t UriQueryParams::count() const
        {
            return params_.size();
        }

        void UriQueryParams::clear()
        {
            params_.clear();
        }

        void UriQueryParams::add(const UriQueryParam& param)
        {
            params_.push_back(param);
        }

        void UriQueryParams::add(const std::string& name, const std::string& value)
        {
            params_.push_back(UriQueryParam());
            params_.back().name_ = name;
            params_.back().value_ = value;
        }

        void UriQueryParams::set(const std::string& name, const std::string& value)
        {
            UriQueryParam* param = find(name);
            if (param)
                param->value_ = value;
            else
                add(name, value);
        }

        bool UriQueryParams::remove(const std::string& name)
        {
            std::vector<UriQueryParam>::iterator iter;
            for (iter = params_.begin(); iter != params_.end(); ++iter) {
                if ((*iter).name_ == name) {
                    params_.erase(iter);
                    return true;
                }
            }
            return false;
        }
    }
}