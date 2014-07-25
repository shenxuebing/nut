﻿/**
 * @file -
 * @author jingqi
 * @date 2012-12-15
 * @last-edit 2014-07-25 21:30:42 jingqi
 * @brief
 */

#ifndef ___HEADFILE_CBBC3AB5_C145_43C1_AAD4_5149A52AF3DD_
#define ___HEADFILE_CBBC3AB5_C145_43C1_AAD4_5149A52AF3DD_

#include <assert.h>
#include <stdlib.h>
#include <map>

#include <nut/threading/sync/spinlock.hpp>
#include <nut/threading/sync/guard.hpp>

namespace nut
{

/**
 * most-recently-used cache
 */
template <typename K, typename V>
class LRUCache
{
    struct Node
    {
        K key;
        V value;
        Node *pre, *next;

        Node(const K& k, const V& v)
            : key(k), value(v), pre(NULL), next(NULL)
        {}
    };

    enum { DEFAULT_CAPACITY = 50 };
    typedef std::map<K,Node*> map_t;

    size_t m_capacity;
    map_t m_map;
    Node *m_list_head, *m_list_end;
    SpinLock m_lock; // 注意，linux下自旋锁不可重入

#ifndef NDEBUG
    size_t m_hit_count, m_miss_count;
#endif

    static inline Node* alloc_node()
    {
        return (Node*)::malloc(sizeof(Node));
    }

    static inline void dealloc_node(Node *p)
    {
        assert(NULL != p);
        ::free(p);
    }

    static inline Node* new_node(const K& k, const V& v)
    {
        Node *p = alloc_node();
        assert(NULL != p);
        new (p) Node(k,v);
        return p;
    }

    static inline void delete_node(Node *p)
    {
        assert(NULL != p);
        p->~Node();
        dealloc_node(p);
    }

    inline void remove_from_list(Node *p)
    {
        assert(NULL != p);
        if (NULL != p->pre)
            p->pre->next = p->next;
        else
            m_list_head = p->next;

        if (NULL != p->next)
            p->next->pre = p->pre;
        else
            m_list_end = p->pre;
    }

    inline void push_list_head(Node *p)
    {
        assert(NULL != p);
        p->next = m_list_head;
        p->pre = NULL;
        if (NULL != m_list_head)
            m_list_head->pre = p;
        else
            m_list_end = p;
        m_list_head = p;
    }

private:
    // invalid methods
    LRUCache(const LRUCache<K,V>&);
    LRUCache<K,V>& operator=(const LRUCache<K,V>&);

public:
    LRUCache()
        : m_capacity(DEFAULT_CAPACITY), m_list_head(NULL), m_list_end(NULL)
#ifndef NDEBUG
        , m_hit_count(0), m_miss_count(0)
#endif
    {}

    LRUCache(size_t capacity)
        : m_capacity(capacity), m_list_head(NULL), m_list_end(NULL)
#ifndef NDEBUG
        , m_hit_count(0), m_miss_count(0)
#endif
    {
        assert(capacity > 0);
    }

    ~LRUCache()
    {
        clear();
    }

    inline size_t size() const
    {
        return m_map.size();
    }

    inline size_t capacity() const
    {
        return m_capacity;
    }

    inline void set_capacity(size_t capacity)
    {
        assert(capacity > 0);
        m_capacity = capacity;
    }

    void put(const K& k, const V& v)
    {
        Guard<SpinLock> g(&m_lock);

        typename map_t::const_iterator const n = m_map.find(k);
        if (n == m_map.end())
        {
            Node *const p = new_node(k,v);
            m_map.insert(std::pair<K,Node*>(k,p));
            push_list_head(p);

            while (m_map.size() > m_capacity)
            {
                assert(NULL != m_list_end);
                typename map_t::iterator const nn = m_map.find(m_list_end->key);
                assert(nn != m_map.end());
                Node *const pp = nn->second;
                assert(NULL != pp);
                m_map.erase(nn);
                remove_from_list(pp);
                delete_node(pp);
            }
            return;
        }

        Node *const p = n->second;
        assert(NULL != p);
        p->value = v;
        remove_from_list(p);
        push_list_head(p);
    }

    void remove(const K& k)
    {
        Guard<SpinLock> g(&m_lock);

        typename map_t::iterator const n = m_map.find(k);
        if (n == m_map.end())
            return;

        Node *const p = n->second;
        assert(NULL != p);
        m_map.erase(n);
        remove_from_list(p);
        delete_node(p);
    }

    inline bool has_key(const K& k)
    {
        return m_map.find(k) != m_map.end();
    }

    bool get(const K& k, V *out)
    {
        assert(NULL != out);
        Guard<SpinLock> g(&m_lock);

        typename map_t::const_iterator const n = m_map.find(k);
        if (n == m_map.end())
        {
#ifndef NDEBUG
            ++m_miss_count;
#endif
            return false;
        }

        assert(NULL != n->second);
        *out = n->second->value;

#ifndef NDEBUG
        ++m_hit_count;
#endif
        return true;
    }

    void clear()
    {
        Guard<SpinLock> g(&m_lock);

        Node *p = m_list_head;
        while (NULL != p)
        {
            Node *n = p->next;
            delete_node(p);
            p = n;
        }
        m_list_head = NULL;
        m_list_end = NULL;
        m_map.clear();

#ifndef NDEBUG
        m_hit_count = 0;
        m_miss_count = 0;
#endif
    }
};

}

#endif
