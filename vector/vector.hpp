#include <iostream>
#include <utility>
#include <new>
#include <exception>
#include <algorithm>

template<typename T>
class myvector final
{
public:
    myvector();
    myvector(int cap);
    myvector(const myvector& rhs);
    myvector(myvector&& rhs) noexcept;
    myvector& operator=(const myvector& rhs);
    myvector& operator=(myvector&& rhs) noexcept;
    ~myvector();

    auto begin() const;
    auto end() const;
    auto begin();
    auto end();  
    T& operator[](int pos);
    T operator[](int pos) const;
    void push_back(const T& val);

private:
    int size_ = 0;
    int capacity_ = 0;
    T* data_ = nullptr;

    template<typename Arg>
    void emplace_back(Arg&& arg);
};

#include "vector.cpp"