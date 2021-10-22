#pragma once

#include "alloc.h"
#include "construct.h"

namespace MiniSTL{
    //Ĭ�϶���������
    template <class T, class Alloc = __default_alloc>
    class simpleAlloc{
    public: 
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

    public:
    //�����ڴ�ռ�  �ͷ��ڴ�ռ�
        static T* allocate();
        static T* allocate(size_t n);
        static void deallocate(T* ptr);
        static void deallocate(T* ptr,size_t n);

        static void construct(T* ptr);
        static void construct(T* ptr, const T& value);
        static void destory(T* ptr);
        static void destory(T* first, T* last);
    };

    //��̬����
    template<class T,class Alloc>
    //�޲η���  �����Ƿ���һ���СΪ��ǰ���͵Ŀռ�
    T* simpleAlloc<T,Alloc>::allocate(){
        return reinterpret_cast<T*>(Alloc::allcate(sizeof(T)));
    }
    
    template<class T,class Alloc>
    //size n ����
    T* simpleAlloc<T,Alloc>::allocate(size_t n){
        if(n == 0) {
            return 0;
        }
        return reinterpret_cast<T*>(Alloc::allocate(sizeof(T) * n));//���䵱ǰ����*n��Ŀռ�
    }

    //deallocate�ͷ�
    template<class T, class Alloc>
    void simpleAlloc<T,Alloc>::deallocate(T* ptr){
        Alloc::deallocate(reinterpret_cast<void*>(ptr),sizeof(T));
        //�ͷ�ptrָ��� һ��������ô��Ŀռ�
    }

    template<class T,class Alloc>
    void simpleAlloc<T,Alloc>::deallocate(T* ptr,size_t n){
        //�ͷ�ptrָ���  n��ô��Ŀռ�
        if(n == 0){
            return ;
        }
        Alloc::deallocate(reinterpret_cast<void*>(ptr),sizeof(T) * n);
        //�ͷŴ�ptrָ��ĵط���ʼ  n*������ô��Ŀռ�
    }

    //����
    template<class T, class Alloc>
    void simpleAlloc<T,Alloc>::construct(T* ptr){
        new (ptr) T();// ��ptrָ���λ��  ����T�Ĺ��캯��
    }

    template<class T,class Alloc>
    void simpleAlloc<T,Alloc>::construct(T* ptr,const T& value){
        new (ptr) T(value);
    }

    //����
    template<class T,class Alloc>
    void simpleAlloc<T,Alloc>::destory(T *ptr){
        //����ptrָ��� 
        ptr->~T();
    }

    template<class T,class Alloc>
    //��������һ�ε��õ�����ָ��Ķ�������� first->~T()
    void simpleAlloc<T,Alloc>::destory(T* first,T * last){
        for(;first != last;++first){
            first->~T();
        }
    }
}