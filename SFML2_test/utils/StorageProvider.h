#pragma once

#include <list>
#include <cassert>

template<class T, class Predicate>
class StorageProvider
{
public:
	StorageProvider(int init_size, int max_size_before_delete, int batch_size, Predicate predicate) :
		min_size(init_size), 
		max_size(max_size_before_delete),
		batch_size(batch_size),
		element_base(T()),
		predicate(predicate) {

			predicate.transform(element_base);

			assert(predicate(element_base) == true);

			elements_created.insert(elements_created.begin(),min_size,element_base); 

			for(auto it = elements_created.begin(); it != elements_created.end(); ++it ) {
				elements_available.push_back(it);
			}
	}

	typename std::list<T>::iterator request_from_storage() {

		if(elements_available.empty()) {
			elements_created.insert(elements_created.begin(), batch_size ,element_base); 
			
			int i = 0;
			for(auto it = elements_created.begin() ; i<batch_size; ++it, ++i ) {
				elements_available.push_back(it);
			}
		}

		auto front = elements_available.front();
		elements_available.pop_front();
		return front;
	}

	typename std::list<T>::iterator default_value() {
		return elements_created.end();
	}

	void return_to_storage(typename std::list<T>::iterator& object) {

		if(object == elements_created.end())
			return;

		assert(predicate(*object) == true);

		if( elements_available.size() >= static_cast<size_t>(max_size)) {
			elements_created.erase(object);
		} else {
			elements_available.push_back(object);
		}
		object = elements_created.end();
	}

	int size_available() const {
		return static_cast<int>(elements_available.size());
	}

	int size_in_use() const {
		return static_cast<int>(elements_created.size()) - static_cast<int>(elements_available.size());
	}


private:
	int min_size;
	int max_size;
	int batch_size;
	std::list<T> elements_created;
	std::list<typename std::list<T>::iterator> elements_available;

	T element_base;

	Predicate predicate;
};

