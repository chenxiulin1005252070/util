//
// Created by hujianzhe on 18-7-24
//

#ifndef	UTIL_CPP_UNORDERED_SET_H
#define	UTIL_CPP_UNORDERED_SET_H

#include "cpp_compiler_define.h"
#if	__CPP_VERSION >= 2011
#include <unordered_set>
#else
#include "../c/datastruct/hashtable.h"
#include <stddef.h>
#include <string>
namespace std {
template <typename K>
class unordered_set {
public:
	typedef	K			key_type;	
	typedef	K			value_type;
	typedef struct Xnode : public hashtable_node_t {
		key_type k;

		Xnode(void) {
			key = (void*)&k;
		}
	} Xnode;

	class iterator {
	friend class unordered_set;
	public:
		iterator(hashtable_node_t* p = NULL) : x(p) {}
		iterator(const iterator& i) : x(i.x) {}
		iterator& operator=(const iterator& i) {
			x = i.x;
			return *this;
		}

		bool operator==(const iterator& i) {
			return i.x == x;
		}

		bool operator!=(const iterator& i) {
			return !operator==(i);
		}

		key_type& operator*(void) {
			return ((Xnode*)x)->k;
		}

		iterator& operator++(void) {
			x = hashtable_next_node(x);
			return *this;
		}
		iterator operator++(int unused) {
			iterator it = *this;
			x = hashtable_next_node(x);
			return it;
		}

	private:
		hashtable_node_t* x;	
	};
	typedef iterator	const_iterator;

private:
	static int keycmp(hashtable_node_t* _n, void* key) {
		return ((Xnode*)_n)->k != *(key_type*)key;
	}

	static unsigned int __key_hash(const std::string& s) {
		const char* str = s.empty() ? "" : s.c_str();
		/* BKDR hash */
		unsigned int seed = 131;
		unsigned int hash = 0;
		while (*str) {
			hash = hash * seed + (*str++);
		}
		return hash & 0x7fffffff;
	}
	static unsigned int __key_hash(size_t n) { return n; }
	static unsigned int __key_hash(const void* p) { return (unsigned int)(size_t)p; }

	static unsigned int keyhash(void* key) {
		return __key_hash(*(key_type*)key);
	}

public:
	unordered_set(void) :
		m_size(0)
	{
		hashtable_init(&m_table, m_buckets, sizeof(m_buckets) / sizeof(m_buckets[0]), keycmp, keyhash);
	}

	unordered_set(const unordered_set<K>& m) {
		hashtable_init(&m_table, m_buckets, sizeof(m_buckets) / sizeof(m_buckets[0]), keycmp, keyhash);
		for (iterator iter = m.begin(); iter != m.end(); ++iter) {
			Xnode* xnode = new Xnode();
			xnode->k = *iter;
			hashtable_insert_node(&m_table, xnode);
			++m_size;
		}
	}

	unordered_set<K>& operator=(const unordered_set<K>& m) {
		if (this == &m) {
			return *this;
		}
		clear();
		for (iterator iter = m.begin(); iter != m.end(); ++iter) {
			Xnode* xnode = new Xnode();
			xnode->k = *iter;
			hashtable_insert_node(&m_table, xnode);
			++m_size;
		}
		return *this;
	}

	~unordered_set(void) { clear(); }

	void clear(void) {
		hashtable_node_t *cur, *next;
		for (cur = hashtable_first_node(&m_table); cur; cur = next) {
			next = hashtable_next_node(cur);
			delete ((Xnode*)cur);
		}
		hashtable_init(&m_table, m_buckets, sizeof(m_buckets) / sizeof(m_buckets[0]), keycmp, keyhash);
		m_size = 0;
	}

	size_t size(void) const { return m_size; };
	bool empty(void) const { return hashtable_first_node((hashtable_t*)&m_table) == NULL; }

	void erase(iterator iter) {
		--m_size;
		hashtable_remove_node(&m_table, iter.x);
		delete (Xnode*)(iter.x);
	}

	size_t erase(const key_type& k) {
		hashtable_node_t* node = hashtable_search_key(&m_table, (void*)&k);
		if (node) {
			hashtable_remove_node(&m_table, node);
			delete (Xnode*)node;
			--m_size;
			return 1;
		}
		return 0;
	}

	iterator find(const key_type& k) const {
		hashtable_node_t* node = hashtable_search_key((hashtable_t*)&m_table, (void*)&k);
		return iterator(node);
	}

	pair<iterator, bool> insert(const key_type& k) {
		hashtable_node_t* n = hashtable_search_key(&m_table, (void*)&k);
		if (n) {
			return pair<iterator, bool>(iterator(n), false);
		}
		Xnode* xnode = new Xnode();
		xnode->k = k;
		hashtable_insert_node(&m_table, xnode);
		++m_size;
		return pair<iterator, bool>(iterator(xnode), true);
	}

	iterator begin(void) const {
		return iterator(hashtable_first_node((hashtable_t*)&m_table));
	}
	iterator end(void) const {
		return iterator();
	}

private:
	hashtable_t m_table;
	hashtable_node_t* m_buckets[11];
	size_t m_size;
};
}
#endif

#endif