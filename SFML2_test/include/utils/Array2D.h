#pragma once

#include <cassert>

#include <SFML/System/Vector2.hpp>


template<class T>
class Array2D
{
public:
	Array2D(int x, int y) :
	data_size(x,y)
	{
		data = new T[x*y <= 0 ? 1 : x*y];
	}

	Array2D(const sf::Vector2i& size) :
	data_size(size)
	{
		data = new T[size.x*size.x <= 0 ? 1 : size.x*size.y];
	}

	~Array2D() {
		delete[] data;
	}

	T& operator()(int x, int y)  {
		assert( x >= 0 && y >= 0 &&  x < data_size.x && y < data_size.y);
		return data[y+data_size.y*x];
	}

	const T& operator()(int x, int y) const {
		assert( x >= 0 && y >= 0 &&  x < data_size.x && y < data_size.y);
		return data[y+data_size.y*x];
	}

	T& operator()(const sf::Vector2i& point)  {
		assert( point.x >= 0 && point.y >= 0 &&  point.x < data_size.x && point.y < data_size.y);
		return data[point.y+data_size.y*point.x];
	}

	const T& operator()(const sf::Vector2i& point) const {
		assert( point.x >= 0 && point.y >= 0 &&  point.x < data_size.x && point.y < data_size.y);
		return data[point.y+data_size.y*point.x];
	}

	const sf::Vector2i& size() const {
		return data_size;
	}

	Array2D(Array2D&& from) :
	data(from.data),
	data_size(from.data_size)
	{
		from.data = 0;
	}

	Array2D& operator=(Array2D&& from) {
		if(this != &from) {
			std::swap(data, from.data);
			std::swap(data_size, from.data_size);
		}
		return *this;
	}


private:
	T* data;
	sf::Vector2i data_size;

private:
	Array2D(Array2D&);
	Array2D& operator=(Array2D&);
	
};

