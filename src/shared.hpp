#ifndef SHARED_PTR
#define SHARED_PTR

#include <cstddef>

template <typename T>
class SharedPtr
{
public:
    SharedPtr();
    SharedPtr(T* p);
    ~SharedPtr();
    SharedPtr(const SharedPtr & other);
    SharedPtr& operator= (const SharedPtr & other);
    operator bool() const;
    bool operator !() const;
    size_t use_count() const;
    T* get() const;
    T& operator* () const;
    T* operator->() const;
    void reset();
    void reset(T* p);

    SharedPtr(SharedPtr&& ptr) noexcept;
    SharedPtr& operator =(SharedPtr&& ptr) noexcept;

    size_t** getcount(){
        return &count;
    }
    void newcount(size_t* pos){
        count=pos;
        (*count)++;
    }


private:
   T* _p;
   size_t* count;
};

//构造函数
template <typename T>
SharedPtr<T>::SharedPtr(){
    _p=nullptr;
    count=nullptr;
}

template <typename T>
SharedPtr<T>::SharedPtr(T* p) : _p(p){
    count=new size_t(1);
}

//复制构造函数
template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr & other) :_p(other._p){
    if(other.count){
        count=other.count;
        ++(*count);
    }else{
        count=nullptr;
    }
}

//=重载
template <typename T>
//这里返回一个引用，防止使用赋值来降低效率
SharedPtr<T>& SharedPtr<T>::operator= (const SharedPtr & other){
    if(this!=&other){
        if(count&&--(*count)==0){
            delete _p;
            delete count;
        }
        _p=other._p;
        if(other.count){
            count=other.count;
            ++(*count);
        }else{
            count=nullptr;
        }
    }
    return *this;//与c++赋值运算保持一致，a=b=c与(a=b)=c
}

//析构函数
template <typename T>
SharedPtr<T>::~SharedPtr(){
    if(count&&--(*count)==0){
        delete _p;
        delete count;
    }
    _p=nullptr;
    count=nullptr;
}

template <typename T>
SharedPtr<T>::operator bool() const{
    if(_p==nullptr){
        return false;
    }
    return true;
}

template <typename T>
bool SharedPtr<T>::operator !() const{
    if(_p==nullptr){
        return true;
    }
    return false;
}

template <typename T>
size_t SharedPtr<T>::use_count() const{
    if(count){
        return *count;
    }else{
        return 0;
    }
}

template <typename T>
T* SharedPtr<T>::get() const{
    return _p;
}

template <typename T>
T& SharedPtr<T>::operator* () const{
    return *_p;
}

template <typename T>
T* SharedPtr<T>::operator->() const{
    return _p;
}

template <typename T>
void SharedPtr<T>::reset(){
    if(count&&--(*count)==0){
        delete count;
        delete _p;
    }
    _p=nullptr;
    count=nullptr;
}

template <typename T>
void SharedPtr<T>::reset(T* p){
    if(count&&--(*count)==0){
        delete count;
        delete _p;
    }
    if(p){
        _p=p;
        count=new size_t(1);
    }else{
        _p=nullptr;
        count=nullptr;
    }
}

template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& ptr) noexcept : _p(ptr._p),count(ptr.count){
    ptr._p=nullptr;
    ptr.count=nullptr;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator =(SharedPtr&& ptr) noexcept {
    if(&ptr!=this){
        if(count&&--(*count)==0){
            delete count;
            delete _p;
        }

        count=ptr.count;
        ptr.count=nullptr;
        _p=ptr._p;
        ptr._p=nullptr;
    }
    return *this;
}

//类外函数
template<typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

#endif //SHARED_PTR
