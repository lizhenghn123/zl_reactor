// ***********************************************************************
// Filename         : ConcurrentHashMap.h
// Author           : LIZHENG
// Description      : 模仿java.util.concurrent库并发hashmap及Mircosoft的concurrent_unordered_map而写的一个简单并发hashmap（分段锁）
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_CONCURRENTHASHMAP_H
#define ZL_CONCURRENTHASHMAP_H
#include <vector>
#include <list>
#include <utility>
#include "thread/Mutex.h"
namespace zl { namespace thread {
    
template<typename Key, typename Value, typename Hash = std::hash<Key>, typename Equality = std::equal_to<Key> >
class ConcurrentHashMap
{
public:
    typedef Key             key_type;
    typedef Value           mapped_type;
    typedef Hash            hash_type;
    typedef Equality        equality_type;
    
    typedef zl::thread::Mutex                   mutex_type;   // FIXME use reader-writer lock
    typedef zl::thread::LockGuard<mutex_type>   lock_guard;
        
public:
    explicit ConcurrentHashMap(size_t num_buckets = 19, const Hash& hasher = Hash(), const Equality& equaler = Equality())
        : buckets_(num_buckets)
        , hasher_(hasher)
        , equaler_(equaler)
    {
        for (size_t i = 0; i < num_buckets; ++i)
        {
            buckets_[i] = new bucket_type(equaler);
        }
    }

    ConcurrentHashMap(ConcurrentHashMap const& other) = delete;
    ConcurrentHashMap& operator=(ConcurrentHashMap const& other) = delete;

    /// find the key is or not in the map
    bool contain(const Key& key) const
    {
        return get_bucket(key).contain(key);
    }

    /// Find the key's corresponding value.
    /// Return the value if find, or default_value if not find.
    Value get(const Key& key, const Value& default_value = Value()) const
    {
        return get_bucket(key).get(key, default_value);
    }

    /// add the key and the value
    void put(const Key& key, const Value& value)
    {
        get_bucket(key).put(key, value);
    }

    /// If the specified key is not already associated with a value, associate it with the given value.
    /// Return the previous value associated with the specified key, or Value() if there was no mapping for the key
    Value putIfAbsent(const Key& key, const Value& value)
    {
        return get_bucket(key).putIfAbsent(key, value);;
    }

    /// Removes the key (and its corresponding value) from this map. This method does nothing if the key is not in the map.
    Value remove(const Key& key)
    {
        return get_bucket(key).remove(key);
    }

    /// Removes the entry for a key only if currently mapped to a given value.
    /// Return true if the value was removed
    bool remove(const Key& key, const Value& exceptValue)
    {
        return get_bucket(key).remove(key, exceptValue);
    }

    size_t size() const
    {
        size_t size = 0;
        for (size_t i = 0; i < buckets_.size(); ++i)
        {
            size += buckets_[i].size();
        }
        return size;
    }

    std::map<Key, Value> map() const
    {
        std::vector<lock_guard> locks;
        for (size_t i = 0; i < buckets_.size(); ++i)
        {
            locks.push_back(buckets_[i].mutex_);
        }
        std::map<Key, Value> res;
        for (size_t i = 0; i < buckets_.size(); ++i)
        {
            for (typename bucket_type::bucket_iterator it = buckets_[i].data_.begin(); it != buckets_[i].data_.end(); ++it)
            {
                res.insert(*it);
            }
        }
        return res;
    }

private:
    class bucket_type
    {
    private:
        typedef std::pair<Key, Value>           bucket_value;
        typedef std::list<bucket_value>         bucket_data;
        typedef typename bucket_data::iterator  bucket_iterator;
        typedef typename bucket_data::const_iterator  const_bucket_iterator;

    public:
        bucket_type(const Equality& equaler) : equaler_(equaler)
        {
        }

        bool contain(const Key& key) const
        {
            lock_guard lock(mutex_);
            const_bucket_iterator iter = find(key);
            return iter != data_.end();
        }

        Value get(const Key& key, const Value& default_value) const
        {
            lock_guard lock(mutex_);
            const_bucket_iterator iter = find(key);
            return (iter == data_.end()) ? default_value : iter->second;
        }

        void put(const Key& key, const Value& value)
        {
            lock_guard lock(mutex_);
            bucket_iterator const iter = find(key);
            if (iter == data_.end())
            {
                data_.push_back(bucket_value(key, value));
            }
            else
            {
                iter->second = value;
            }
        }

        Value putIfAbsent(const Key& key, const Value& value)
        {
            lock_guard lock(mutex_);
            const bucket_iterator iter = find(key);
            if (iter == data_.end())
            {
                data_.push_back(bucket_value(key, value));
                return value;
            }
            else
            {
                return iter->second;
            }
        }

        Value remove(const Key& key)
        {
            Value v;
            lock_guard lock(mutex_);
            const bucket_iterator iter = find(key);
            if (iter != data_.end())
            {
                v = iter->second;
                data_.erase(iter);
            }
            return v;
        }

        bool remove(const Key& key, const Value& exceptValue)
        {
            lock_guard lock(mutex_);
            const bucket_iterator iter = find(key);
            if (iter != data_.end()& & iter.second == exceptValue)
            {
                data_.erase(iter);
                return true;
            }
            return false;
        }

        size_t size() const
        {
            lock_guard lock(mutex_);
            return data_.size();
        }

    private:
        bucket_iterator find(const Key& key)
        {
            return std::find_if(data_.begin(), data_.end(), [&](const bucket_value& item)
            {
                return equaler_(item.first, key);
            });
        }

        const_bucket_iterator find(const Key& key) const
        {
            return std::find_if(data_.begin(), data_.end(), [&](bucket_value const&  item)
            {
                return equaler_(item.first, key);
            });
        }
        
    private:    
        Equality     equaler_;
        bucket_data  data_;
        mutable zl::thread::Mutex mutex_;
    };

    bucket_type& get_bucket(const Key& key) const
    {
        const std::size_t bucket_index = hasher_(key) % buckets_.size();
        return *buckets_[bucket_index];
    }

private:
    Hash                           hasher_;
    Equality                       equaler_;
    std::vector<bucket_type*>      buckets_;
};

}  }
#endif  /* ZL_CONCURRENTHASHMAP_H */
