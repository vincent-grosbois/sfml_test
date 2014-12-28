#pragma once

#include <unordered_set>

template<class T>
struct Equal: ::std::binary_function<T, T, bool> {
    bool operator()(T const& left, T const& right) const {
        ::std::equal_to<T> equal;

        return equal(left, right);
    }
};

template<class T>
struct Equal<T*> : ::std::binary_function<T*, T*, bool> {
    bool operator()(T* const & left, T* const & right) const {
        Equal<T> equal;

        return equal(*left, *right);
    }
};

template<class T>
struct Hash: ::std::unary_function<T, ::std::size_t> {
    ::std::size_t operator()(T const & value) const {
        ::std::hash<T> hash;

        return hash(value);
    }
};

template<class T>
struct Hash<T*> : ::std::unary_function<T*, ::std::size_t> {
    ::std::size_t operator()(T* const & value) const {
        Hash<T> hash;

        return hash(*value);
    }
};

template<class T>
class ResourceContainer {
public:



    bool contains(T* element) const {
        return s_.find(element) != s_.end();
    }

    bool insert(T* element) {
        return s_.insert(element).second;
    }

	void unloadAll() {

		for(auto obj : s_) {		
			delete obj;
		}
		s_.clear();

	}

	~ResourceContainer() {
		for(auto obj : s_) {		
			delete obj;
		}
	}


private:
    std::unordered_set<T*, Hash<T*>, Equal<T*>> s_;
};