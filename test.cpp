#include <stdio.h>
#include <new>
#include <cstdlib>

class NewHandlerHolder {
public:
    explicit NewHandlerHolder(std::new_handler nh): handler(nh) {}
    ~NewHandlerHolder() {std::set_new_handler(handler);}
private:
    std::new_handler handler;
    NewHandlerHolder(const NewHandlerHolder&);
    NewHandlerHolder& operator=(const NewHandlerHolder&);
};

template<typename T>
class NewHandlerSupport {
public:
    static std::new_handler set_new_handler(std::new_handler p) throw();
    static void* operator new(std::size_t size) throw(std::bad_alloc);
private:
    static std::new_handler currentHandler;
};

class Widget: public NewHandlerSupport<Widget> {
private:
    char* a[10000000000000];
};

template<typename T>
std::new_handler NewHandlerSupport<T>::set_new_handler(std::new_handler p) throw()
{
    std::new_handler oldHandler = currentHandler;
    currentHandler = p;
    return oldHandler;
}

template<typename T>
void* NewHandlerSupport<T>::operator new(std::size_t size) throw(std::bad_alloc)
{
    NewHandlerHolder h(std::set_new_handler(currentHandler));
    return ::operator new(size);
}

template<typename T>
std::new_handler NewHandlerSupport<T>::currentHandler = 0;

void outOfMem()
{
    printf("Unable to satisfy request for memory\n");
    std::abort();
}

int main()
{
    Widget::set_new_handler(outOfMem);
    Widget* test = new Widget;
    delete[] test;
}
