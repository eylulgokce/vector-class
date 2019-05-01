#ifndef VECTOR_H
#define VECTOR_H

#include<initializer_list>
#include<stdexcept>
#include<iostream>

using namespace std; 

template<typename T>

class Vector {
public:
	class Iterator; // forward declaration
	class ConstIterator;
	using value_type = T;
	using size_t = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type& ;
	using const_reference = const value_type&;
	using pointer = value_type* ;
	using const_pointer = const value_type*;
	using iterator = Vector::Iterator;
	using const_iterator = Vector::ConstIterator;

private:
	static constexpr size_t min_sz{ 5 };
	size_t max_sz; // Kapazitat
	size_t sz; //next free position
	pointer values = nullptr;

	void initialize(size_t size)
	{
		deinitialize();
		sz = 0;
		this->max_sz = size < min_sz ? min_sz : size;
		this->values = new value_type[max_sz];
	}

	void deinitialize()
	{
		if (values != nullptr)
		{
			delete[] values;
			values = nullptr;
		}
	}

public:
	//Default constructor
	Vector() : Vector(min_sz) {}

	//Size Constructor
	Vector(size_t size) {
	
		initialize(size);
	}

	// Initializer List constructor
	Vector(initializer_list<value_type> il) : Vector(il.size()) {

		for (const auto & e : il) {
			values[sz++] = e;
		}
	}

	//Copy Constructor
	Vector(const Vector& v) : Vector(v.sz)
	{
		for (const auto & e : v) {
			values[sz++] = e;
		}
	}

	//Copy Assignment Operator
	Vector& operator=(const Vector& v) {
		initialize(v.sz);
		for (const auto & e : v) {
			values[sz++] = e;
		}
		return *this;
	}

	//Pointers (Methoden begin() end())
	Iterator begin() { return Iterator{ values, this }; }
	Iterator end() { return Iterator{ values + sz, this }; }
	const_iterator begin() const { return ConstIterator{ values, this }; }
	const_iterator end() const { return ConstIterator{ values + sz, this }; }


	//Destructor
	~Vector() {
		deinitialize();
	}

	//Methoden
	size_t size() {
		return sz;
	}

	bool empty() {
		return sz == 0;
	}

	void clear() {
		sz = 0;
	}

	void pop_back() {
		if (sz <= 0)
			throw runtime_error("Can't pop empty vector!");
		--sz;
	}
	//Reserve
	void reserve(size_t new_max_sz) {
		//min platz absicherung
		if (new_max_sz < sz)
			throw runtime_error("Can't reserve less than the current size!");

		pointer buf = new value_type[new_max_sz];

		for (size_t i = 0; i < sz; i++)
			buf[i] = values[i];

		deinitialize();
		max_sz = new_max_sz;
		values = buf;
	}

	//Push_back
	void push_back(value_type v) {
		if (sz == max_sz) {
			reserve(max_sz * 2);
		}

		values[sz++] = v;
	}
	//Const reference operator[]
	const_reference operator[](size_t idx) const {
		if (idx < 0 || idx >= sz)
			throw runtime_error("WOOPS");

		return values[idx];
	}

	//Operator []
	reference operator[](size_t idx) {
		if (idx < 0 || idx >= sz)
			throw runtime_error("WOOPS");

		return values[idx];
	}

	//Schrink to fit
	void shrink_to_fit() {
		reserve(sz);
	}

	//Ostream & Print
	friend ostream& operator<<(ostream &o, const Vector<T> &v) {
		return v.print(o);
	}

	ostream& print(ostream &o) const {
		o << '[';
		for (size_t i = 0; i < sz; i++) {
			if (i > 0)
				o << ", ";
			o << values[i];
		}
		o << ']';
		return o;
	}

	Vector::iterator insert(Vector::const_iterator pos, Vector::const_reference val) {
		auto index = pos - begin();
		if (index > sz)
			throw runtime_error("Iterator out of bounds");

		if (sz >= max_sz)
			reserve(max_sz * 2); //max_sz*2+10, wenn Ihr Container max_sz==0 erlaubt

		// 1,2,3,4
		for (size_t i = sz; i > index; i--)
			values[i] = values[i-1];
		
		values[index] = val;
		++sz;
		return Vector::iterator{ values + index, this };
	}

	Vector::iterator erase(Vector::const_iterator pos) {
		auto index = pos - begin();
		if (index > sz)
			throw runtime_error("Iterator out of bounds");

		for (size_t i = index; i < sz-1; ++i)
			values[i] = values[i + 1];

		--sz;
		return Vector::iterator{ values + index, this };
	}

	//ITERATOR CLASS
	class Iterator {
	public:
		using value_type = Vector::value_type;
		using reference = Vector::reference;
		using pointer = Vector::pointer;
		using difference_type = Vector::difference_type;
		using iterator_category = std::forward_iterator_tag;
	private:
		pointer current;
		Vector *vector;
	public:
		//Konstruktoren
		Iterator(pointer ptr, Vector *v) : current{ ptr }, vector{ v } {};
		operator const_iterator() const { return const_iterator{ current, vector }; }
		//Operatoren

		// Prefix ++
		Iterator &operator++() {
			if (vector->end() == *this)
				throw runtime_error("too much");
			++current;
			return *this;
		}

		// Postfix++
		Iterator operator++(int) {
			if (vector->end() == *this)
				throw runtime_error("too much");

			Iterator temp = *this;
			++current;
			return temp;
		}

		reference operator*() {
			if (vector->end() == *this)
				throw runtime_error("too much");

			return *current;
		}

		pointer operator->() {
			if (vector->end() == *this)
				throw runtime_error("too much");

			return current;
		}

		friend bool const operator==(const Vector::Iterator &lop, const Vector::Iterator &rop) {
			return lop.current == rop.current && lop.vector == rop.vector;
		}

		friend bool const operator!=(const Vector::Iterator &lop, const Vector::Iterator &rop) {
			return !(lop == rop);
		}

		//Deklaration 
		friend Vector::difference_type operator-(const Vector::Iterator& lop, const Vector::Iterator& rop) {
			return lop.current - rop.current;
		}
	};

	//CONST ITERATOR CLASS
	class ConstIterator {
	public:
		using value_type = Vector::value_type;
		using reference = Vector::const_reference;
		using pointer = Vector::const_pointer;
		using difference_type = Vector::difference_type;
		using iterator_category = std::forward_iterator_tag;
	private:
		pointer current;
		const Vector *vector;
	public:
		//Konstruktoren
		ConstIterator(pointer ptr, const Vector *v) : current{ ptr }, vector{ v } {}
		//Operatoren

		// prefix
		ConstIterator &operator++() {
			if (vector->end() == *this)
				throw runtime_error("over");
			++current;
			return *this;
		}

		//postfix
		ConstIterator operator++ (int) {
			if (vector->end() == *this)
				throw runtime_error("over");

			ConstIterator temp = *this;
			++current;
			return temp;
		}

		// dereference
		const_reference operator*() const {
			if (vector->end() == *this)
				throw runtime_error("over");
			return *current;
		}

		// pointer
		const_pointer operator->() const {
			if (vector->end() == *this)
				throw runtime_error("over");

			return current;
		}

		friend bool operator==(const Vector::ConstIterator &lop, const Vector::ConstIterator &rop) {
			return lop.current == rop.current && lop.vector == rop.vector;
		}

		friend bool operator!=(const Vector::ConstIterator &lop, const Vector::ConstIterator &rop) {
			return !(lop == rop);
		}

		//Deklaration operator -
		friend Vector::difference_type operator-(const Vector::ConstIterator& lop,
			const Vector::ConstIterator& rop) {
			return lop.current - rop.current;
		}
	};


};

#endif VECTOR_H