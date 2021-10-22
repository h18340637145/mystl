#pragma once

#include <new> // placement new 
#include "typeTrits.h"
//
//���ã����ѷ�����ڴ���е��ù��캯������������
    // ���ù��캯��ʹ��pָ�룬��_T1���͵Ĺ��캯�����г�ʼ��
    //��������  �ж�_value_type�����Ƿ���  �޼�ֵ����������   ��pod���Ͷ����޼�ֵ�� ctor��  
    // �����޼�ֵ�� -- ���������������ô���
namespace MiniSTL{
    template<class T1,class T2>
    inline void construct(T1 *p,T2 value){
        new (p) T1(value);
    }

    template<class T>
    inline void destory(T *p){
        p->~T();
    }

    //�跨����traits������������
    template <class ForwardIterator>
    inline void destory(ForwardIterator beg,ForwardIterator end){
        using is_POD_type =  typename _type_traits<ForwardIterator>::is_POD_type;
        //pod���͵�  ��������
        _destory_aux(beg,end,is_POD_type());
    }

    //���Ԫ�ص�value_type������non_trival destructor
    //һ���������  ����pod����_false_type   ��Ҫ����
    template <class ForwardIterator>
    inline void _destory_aux(ForwardIterator beg,ForwardIterator end,
    _false_type){
        for(; beg != end ; ++beg){
            destory(&*beg);   //  *beg  ������ָ�������  &(*beg) �Ե�����ָ�������ȡ��ַ
        }//�Ͼ����������������ĵ�ַ
    }

    //����trival destructor  _true_type   ��pod���� --- �޼�ֵ����---����Ҫ����---��ʵ��
    //�����������������޹�ʹ��   ��ô������������һ��Ч���ϵľ޴��˷�
    template<class ForwardIterator>
    inline void _destory_aux(ForwardIterator,ForwardIterator,_true_type){}

    //���char* wchar_t* ���ػ�  �ַ����� pod��
    inline void destory(char * , char * ){}
    inline void destory(wchar_t*, wchar_t *){}
}