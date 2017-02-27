#pragma once

//
//	Created by ABacker on 9/5/2016
//

#ifndef _FAKELIST_ABACKER_NINI_H_
#define _FAKELIST_ABACKER_NINI_H_

#include <string>
#include <assert.h>
#include <stdexcept>
#include <cstring>

#define _NINI_BEGIN namespace nini {
#define _NINI_END }

_NINI_BEGIN

#define DEBUG									0x00011111
#define DEBUG_RANGE_CHECK						0x00000001
#define DEBUG_UNKONWN_ERR_CHECK					0x00000010
#define DEBUG_NEW_DELETE_CHECK					0x00000100
#define DEBUG_COUNT_ERR_INF						0x00001000
#define DEBUG_PRINT_NODE						0x00010000

#define DEBUG_PREV_PTR							0x00100000
//TODO Add prev_ptr

#define DEFAULT_SIZE_OF_EACH_NODE				(1000000 / sizeof(_Ty))
#define DEFAULT_SIZE_OF_EACH_NODE_CHAR			 1000000

#define NOEXCEPT throw()

    typedef unsigned int size_type;


//TEMPLATE CLASS _FakeList_node_ptr
    template<class _Ty>
    class _FakeList_node_ptr {
    public:
        _FakeList_node_ptr() : _Data(NULL), _Ref(0) {
        }

        explicit _FakeList_node_ptr(_Ty *data) : _Data(data), _Ref(1) {
        }

        ~_FakeList_node_ptr() {
#if(DEBUG & DEBUG_UNKONWN_ERR_CHECK)
            assert(_Ref == 0);//_Data should be deleted only when _Ref == 0
#endif
            if (_Data)
                delete[] _Data;
        }

    public:
        _Ty *_Data;
        size_type _Ref;
    };


//TEMPLATE CLASS _FakeList_node
    template<class _Ty>
    class _FakeList_node {
    public:
        typedef _FakeList_node_ptr<_Ty> ptr;
        typedef _FakeList_node<_Ty> node;

        _FakeList_node()
                : _Size(0), _Offset(0), _Next(NULL), _Ptr(NULL) {
        }

        _FakeList_node(_Ty *_Data, size_type _Size, size_type _Offset = 0, node *_Next = NULL)
                : _Size(_Size), _Offset(_Offset), _Next(_Next), _Ptr(new ptr(_Data)) {
        }

        _FakeList_node(const node &_Node)
                :_Size(_Node._Size), _Offset(_Node._Offset), _Next(_Node._Next), _Ptr(_Node._Ptr) {
            if (/*this != &_Node && */_Ptr)
                ++_Ptr->_Ref;
        }

        _FakeList_node(node &&_Node)
                :_Size(_Node._Size), _Offset(_Node._Offset), _Next(_Node._Next), _Ptr(_Node._Ptr) {
            _Node._Tidy();
        }

        node &operator=(const node &_Right) {
            _Dis();

            this->_Size = _Right._Size;
            this->_Offset = _Right._Offset;
            this->_Next = _Right._Next;
            this->_Ptr = _Right._Ptr;

            if (this->_Ptr)
                ++(this->_Ptr->_Ref);

            return (*this);
        }

        node &operator=(node &&_Right) {
            _Dis();

            this->_Size = _Right._Size;
            this->_Offset = _Right._Offset;
            this->_Next = _Right._Next;
            this->_Ptr = _Right._Ptr;

            _Right._Tidy();

            return (*this);
        }

        node &assign(_Ty *_Data, size_type _Size, size_type _Offset = 0, node *_Next = NULL) {
            if (_Ptr && (--_Ptr->_Ref == 0)) {
                delete[] _Ptr->_Data;
                this->_Ptr->_Data = _Data;
                this->_Ptr->_Ref = 1;
            }
            else _Ptr = new ptr(_Data);

            this->_Size = _Size;
            this->_Offset = _Offset;
            this->_Next = _Next;

            return (*this);
        }

        node &swap(const node &_Node) {
            std::swap(this->_Size, _Node._Size);
            std::swap(this->_Offset, _Node._Offset);
            std::swap(this->_Next, _Node._Next);
            std::swap(this->_Ptr, _Node._Ptr);

            return (*this);
        }

        _Ty &operator[](size_type _Pos) {

#if(DEBUG & DEBUG_RANGE_CHECK)
            if (this->_Size < _Pos + 1)
                throw std::out_of_range("FakeList");
#endif

            return _Ptr->_Data[_Pos + _Offset];
        }

        const _Ty &operator[](size_type _Pos) const {

#if(DEBUG & DEBUG_RANGE_CHECK)
            if (this->_Size < _Pos + 1)
                throw std::out_of_range("FakeList");
#endif

            return _Ptr->_Data[_Pos + _Offset];
        }

        ~_FakeList_node() {
            _Dis();
        }

        void _Tidy() {
            this->_Size = 0;
            this->_Offset = 0;
            this->_Next = NULL;
            this->_Ptr = NULL;
        }

        void _Dis() {
            if (_Ptr && (--_Ptr->_Ref == 0)) {
                delete _Ptr;
            }
        }

    public:
        size_type _Size;
        size_type _Offset;
        node *_Next;

#if(DEBUG & DEBUG_PREV_PTR)
        node *_Prev;
#endif

    private:
        ptr *_Ptr;
    };


//TEMPLATE CLASS _FakeList_const_iterator
    template<class _Ty>
    class _FakeList_const_iterator {
    public:
        typedef _FakeList_node<_Ty> node;
        typedef _FakeList_const_iterator<_Ty> const_iterator;

        _FakeList_const_iterator()
                : _Curnode(NULL), _Curpos(0) {
        }

        _FakeList_const_iterator(node *_Curnode, size_type _Curpos)
                : _Curnode(_Curnode), _Curpos(_Curpos) {
        }

        const_iterator operator++() {
            if (_Curpos + 1 < _Curnode->_Size) ++_Curpos;
            else {
                _Curnode = _Curnode->_Next;
                _Curpos = 0;

#if(DEBUG & DEBUG_PRINT_NODE)
                printf(",");
#endif
            }
            return (*this);
        }

        const_iterator &operator++(int) {
            const_iterator _Iter = *this;

            ++(*this);

            return _Iter;
        }

        const_iterator operator+(size_type _Count) {
            const_iterator _Iter = *this;

            _Iter += _Count;

            return _Iter;
        }

        const_iterator &operator+=(size_type _Count) {
            if (_Curnode->_Size <= _Curpos + _Count) {
                _Count -= (_Curnode->_Size - _Curpos);
                _Curpos = 0;
                _Curnode = _Curnode->_Next;

                while (_Curnode->_Size <= _Curpos + _Count) {
                    _Count -= _Curnode->_Size;
                    _Curnode = _Curnode->_Next;
                }
            }
            _Curpos += _Count;

            return (*this);
        }

        size_type operator-(const const_iterator &_Right) const {
            //*this-_Right, [Right,this)
            if (this->_Curnode == _Right._Curnode) {
                return this->_Curpos - _Right._Curpos;
            }

            size_type _Count = (_Right._Curnode->_Size - _Right._Curpos) + this->_Curpos;

            node *_Tmp = _Right._Curnode->_Next;
            while (_Tmp != this->_Curnode) {
                _Count += _Tmp->_Size;
                _Tmp = _Tmp->_Next;
            }

            return _Count;
        }

        const _Ty *operator->() const {
            return &((*_Curnode)[_Curpos]);
        }

        const _Ty &operator*() const {
            return (*_Curnode)[_Curpos];
        }

        bool operator==(const const_iterator &_Right) const {
            return this->_Curpos == _Right._Curpos && this->_Curnode == _Right._Curnode;
        }

        bool operator!=(const const_iterator &_Right) const {
            return this->_Curpos != _Right._Curpos || this->_Curnode != _Right._Curnode;
        }

        node *_GetCurnode() const {
            return _Curnode;
        }

        size_type _GetCurpos() const {
            return _Curpos;
        }

        void _SetCurnode(node *_Curnode) {
            this->_Curnode = _Curnode;
        }

        void _SetCurpos(size_type _Curpos) {
            this->_Curpos = _Curpos;
        }

    protected:
        node *_Curnode;
        size_type _Curpos;
    };


//TEMPLATE CLASS _FakeList_iterator
    template<class _Ty>
    class _FakeList_iterator :public _FakeList_const_iterator<_Ty> {
    public:
        typedef _FakeList_node<_Ty> node;
        typedef _FakeList_iterator<_Ty> iterator;
        typedef _FakeList_const_iterator<_Ty> base;

        _FakeList_iterator()
                :base() {
        }

        _FakeList_iterator(node *_Curnode, size_type _Curpos)
                :base(_Curnode, _Curpos) {
        }

        iterator operator++() {
            ++(*static_cast<base *>(this));

            return (*this);
        }

        iterator &operator++(int) {
            iterator _Iter = *this;

            ++(*this);

            return _Iter;
        }

        iterator operator+(size_type _Count) {
            iterator _Iter = *this;
            _Iter += _Count;

            return _Iter;
        }

        iterator &operator+=(size_type _Count) {
            *static_cast<base *>(this) += _Count;

            return (*this);
        }

        _Ty *operator->() const {
            return &((*base::_Curnode)[base::_Curpos]);
        }

        _Ty &operator*() const {
            return (*base::_Curnode)[base::_Curpos];
        }
    };


//template<class _Ty>
//class _FakeList_ptr {
//
//	typedef _FakeList_node<_Ty> node;
//
//public:
//	_FakeList_ptr()
//		:_Size(0), _Front(NULL), _Back(NULL), _Ref(0) {
//	}
//
//public:
//	size_type _Size;
//	node *_Front;
//	node *_Back;
//	size_type _Ref;
//};


//TEMPLATE CLASS FakeList
    template<class _Ty>
    class FakeList {
    public:
        typedef _FakeList_node<_Ty> node;
        typedef _FakeList_iterator<_Ty> iterator;
        typedef _FakeList_const_iterator<_Ty> const_iterator;

        FakeList()
                :_Size(0), _Front(NULL), _Back(NULL) {
        }

        FakeList(const _Ty *_Elem, size_type _Count)
                : _Front(NULL) {
            assign(_Elem, _Count);
        }

        FakeList(_Ty *&&_Elem, size_type _Count)
                : _Front(NULL) {
            assign(std::move(_Elem), _Count);
        }

        FakeList(FakeList &&_FakeList)
                : _Front(NULL) {
            assign(std::move(_FakeList));
        }

        FakeList(const FakeList &_FakeList)
                : _Front(NULL) {
            assign(_FakeList);
        }

        FakeList &operator=(FakeList &&_Right) {
            return assign(std::move(_Right));
        }

        FakeList &operator=(const FakeList &_Right) {
            return assign(_Right);
        }

        _Ty &operator[](size_type _Pos) {

#if(DEBUG & DEBUG_RANGE_CHECK)
            if (_Pos + 1 > _Size)
                throw std::out_of_range("FakeList");
#endif

            node *_Node = _Front;
            while (_Node != NULL) {
                if (_Node->_Size > _Pos)
                    return (*_Node)[_Pos];

                _Pos -= _Node->_Size;
                _Node = _Node->_Next;
            }
            throw std::out_of_range("FakeList");
        }

        const _Ty &operator[](size_type _Pos) const {

#if(DEBUG & DEBUG_RANGE_CHECK)
            if (_Pos + 1 > _Size)
                throw std::out_of_range("FakeList");
#endif
            node *_Node = _Front;
            while (_Node != NULL) {
                if (_Node->_Size > _Pos)
                    return (*_Node)[_Pos];

                _Pos -= _Node->_Size;
                _Node = _Node->_Next;
            }
            throw std::out_of_range("FakeList");
        }

        FakeList &assign(const _Ty *_Elem, size_type _Count) {
            _Ty *data = _Clone(_Elem, _Count);

            if (_Front == NULL)
                _Front = new node(data, _Count);
            else {
                _Tidy(_Front->_Next);
                _Front->assign(data, _Count);
            }

            _Size = _Count;
            _Back = _Front;

            return *this;
        }

        FakeList &assign(_Ty *&&_Elem, size_type _Count) {
            if (_Front == NULL)
                _Front = new node(_Elem, _Count);
            else {
                _Tidy(_Front->_Next);
                _Front->assign(_Elem, _Count, 0, NULL);
            }

            _Elem = NULL;

            _Size = _Count;
            _Back = _Front;

            return *this;
        }

        FakeList &assign(FakeList &&_FakeList) {
            std::swap(this->_Size, _FakeList._Size);
            std::swap(this->_Front, _FakeList._Front);
            std::swap(this->_Back, _FakeList._Back);

            return *this;
        }

        FakeList &assign(const FakeList &_FakeList) {
            this->_Size = _FakeList._Size;
            this->_Front = _FakeList._Front;
            this->_Back = _FakeList._Back;

            return (*this);
        }

        FakeList &insert(const _Ty *_Elem, size_type _Count, size_type _Pos) {

#if(DEBUG & DEBUG_RANGE_CHECK)
            if (_Size < _Pos)
                throw std::out_of_range("FakeList");
#endif
            if (_Size == _Pos)return push_back(_Elem, _Count);

            if (_Pos == 0)return push_front(_Elem, _Count);

            _Ty *_NewData = _Clone(_Elem, _Count);
            node *_Node = _Posnode(&_Pos);

            _Insert(_NewData, _Count, _Node, _Pos);

            _Size += _Count;

            return (*this);
        }

        FakeList &insert(_Ty *&&_Elem, size_type _Count, size_type _Pos) {

#if(DEBUG & DEBUG_RANGE_CHECK)
            if (_Size < _Pos)
                throw std::out_of_range("FakeList");
#endif
            if (_Size == _Pos)return push_back(_Elem, _Count);

            if (_Pos == 0)return push_front(_Elem, _Count);

            node *_Node = _Posnode(&_Pos);

            _Insert(_Elem, _Count, _Node, _Pos);

            _Elem = NULL;

            _Size += _Count;

            return (*this);
        }

        //this will disable the iterator
        FakeList &insert(const _Ty *_Elem, size_type _Count, const_iterator _Iter) {
            if (_Iter == this->begin())return push_front(_Elem, _Count);

            if (_Iter == this->end())return push_back(_Elem, _Count);

            _Ty *_NewData = _Clone(_Elem, _Count);

            _Insert(_NewData, _Count, _Iter._GetCurnode(), _Iter._GetCurpos());

            _Size += _Count;

            return (*this);
        }

        //this will disable the iterator
        FakeList &insert(_Ty *&&_Elem, size_type _Count, const_iterator _Iter) {
            if (_Iter == this->begin())return push_front(_Elem, _Count);

            if (_Iter == this->end())return push_back(_Elem, _Count);

            _Insert(_Elem, _Count, _Iter._GetCurnode(), _Iter._GetCurpos());

            _Elem = NULL;

            _Size += _Count;

            return (*this);
        }

        //FakeList &insert(FakeList &&_FakeList, size_type _Pos) {

        //}

        FakeList &push_front(const _Ty *_Elem, size_type _Count) {
            _Ty *_NewData = _Clone(_Elem, _Count);

            node *_Node = new node(_NewData, _Count);

            _Node->_Next = _Front;
            _Front = _Node;

            _Size += _Count;

            return (*this);
        }

        FakeList &push_front(_Ty *&&_Elem, size_type _Count) {
            node *_Node = new node(_Elem, _Count);

            _Elem = NULL;

            _Node->_Next = _Front;
            _Front = _Node;

            _Size += _Count;

            return (*this);
        }

        FakeList &push_front(const _Ty &_Elem) {
            return push_front(&_Elem, 1);
        }

        FakeList &push_front(_Ty &&_Elem) {
            return push_front(std::move(&_Elem), 1);
        }

        FakeList &push_front(FakeList &&_FakeList) {
            _FakeList._Back->_Next = this->_Front;
            this->_Front = _FakeList._Front;

            _FakeList._Front = NULL;
            _FakeList._Back = NULL;

            this->_Size += _FakeList._Size;

            return (*this);
        }

        FakeList &push_back(const _Ty *_Elem, size_type _Count) {
            _Ty *_NewData = _Clone(_Elem, _Count);

            if (_Front == NULL) {
                _Front = new node(_NewData, _Count);
                _Back = _Front;
            }
            else {
                _Back->_Next = new node(_NewData, _Count);
                _Back = _Back->_Next;
            }

            _Size += _Count;

            return (*this);
        }

        FakeList &push_back(_Ty *&&_Elem, size_type _Count) {
            if (_Front == NULL) {
                _Front = new node(_Elem, _Count);
                _Back = _Front;
            }
            else {
                _Back->_Next = new node(_Elem, _Count);
                _Back = _Back->_Next;
            }

            _Elem = NULL;

            _Size += _Count;

            return (*this);
        }

        FakeList &push_back(const _Ty &_Elem) {
            return push_back(&_Elem, 1);
        }

        FakeList &push_back(_Ty &&_Elem) {
            return push_back(std::move(&_Elem), 1);
        }

        FakeList &push_back(FakeList &&_FakeList) {
            this->_Back->_Next = _FakeList._Front;
            this->_Back = _FakeList._Back;

            _FakeList._Front = NULL;
            _FakeList._Back = NULL;

            this->_Size += _FakeList._Size;

            return (*this);
        }

        FakeList &pop_front() {
#if(DEBUG & DEBUG_RANGE_CHECK)
            if (_Size == 0)
                throw std::out_of_range("FakeList");
#endif
            if (--_Front->_Size == 0) {
                node *_Tmp = _Front;
                _Front = _Front->_Next;
                delete _Tmp;
            }
            else ++_Front->_Offset;

            if (--_Size == 0)_Back = NULL;

            return (*this);
        }

        FakeList &pop_back() {
#if(DEBUG & DEBUG_RANGE_CHECK)
            if (_Size == 0)
                throw std::out_of_range("FakeList");
#endif

            if (--_Size == 0)_Front = NULL;

            if (--_Back->_Size == 0) {
                delete _Back;
                _Back = _Prevnode(_Back);
                if (_Back)_Back->_Next = NULL;
            }

            return (*this);
        }

        /*
        FakeList &pop_front_n() {
        return (*this);
        }
        FakeList &pop_back_n() {
        return (*this);
        }
        FakeList &pop_back_node() {
        return (*this);
        }
        FakeList &pop_front_node() {
        #if(DEBUG & DEBUG_RANGE_CHECK)
        if (_Size == 0)
        throw std::out_of_range("FakeList");
        #endif
        return (*this);
        }
        */

        //this will disable iterators
        FakeList &erase(size_type _Begin, size_type _Count) {
            //erase elements [_Begin,_End)
#if(DEBUG &DEBUG_RANGE_CHECK)
            if (_Begin + _Count > _Size)
                throw std::out_of_range("FakeList");
#endif
            node *_L, *_R;

            _Count += _Begin;

            _L = _Posnode(&_Begin);
            _R = _Posnode(&_Count);

            if (_Begin == _L->_Size) {
                _Begin = 0;
                _L = _L->_Next;
            }
            if (_Count == _R->_Size) {
                _Count = 0;
                _R = _R->_Next;
            }

            return erase(const_iterator(_L, _Begin), const_iterator(_R, _Count));
        }

        //this will disable iterators
        FakeList &erase(const_iterator _Begin, const_iterator _End) {
            //erase elements [_Begin,_End)
            if (_Begin == _End) return (*this);

            if (_Begin == this->begin() && _End == this->end()) {
                //erase all
                _Tidy(_Front);
                _Tidy();

                return (*this);
            }

            size_type _Count = 0;

            if (_Begin._GetCurnode() == _End._GetCurnode()) {
                //in one node but not all node
                _Count += _End._GetCurpos() - _Begin._GetCurpos();

                if (_Begin._GetCurpos() == 0) {
                    //from beginning
                    _Begin._GetCurnode()->_Offset += _End._GetCurpos();
                    _Begin._GetCurnode()->_Size -= _End._GetCurpos();
                }
                else {
                    //not from beginning,separate the node,and disable _End iterator
                    node *_Sep_node = new node(*_Begin._GetCurnode());
                    _Sep_node->_Offset += _End._GetCurpos();
                    _Sep_node->_Size -= _End._GetCurpos();
                    _Begin._GetCurnode()->_Size = _Begin._GetCurpos();
                    _Begin._GetCurnode()->_Next = _Sep_node;
                }
            }
            else {
                //in two or more nodes
                node* _PrevL, *_L, *_R;//nodes [_L,_R) will be deleted

                if (_Begin._GetCurpos() == 0) {
                    //erase from beginning of the node,delete it
                    _PrevL = _Prevnode(_Begin._GetCurnode());
                    _L = _Begin._GetCurnode();
                }
                else {
                    _PrevL = _Begin._GetCurnode();
                    _L = _Begin._GetCurnode()->_Next;

                    _Count += _PrevL->_Size - _Begin._GetCurpos();
                    _PrevL->_Size = _Begin._GetCurpos();
                }

                _Count += _End._GetCurpos();
                _R = _End._GetCurnode();
                if (_R) {
                    _R->_Offset += _End._GetCurpos();
                    _R->_Size -= _End._GetCurpos();
                }

                _Count += _Tidy_n(_L, _R);

                if (!_PrevL) {
                    //_Front node will be deleted
                    _Front = _R;
                }
                else {
                    _PrevL->_Next = _R;
                }

                if (_R == NULL) {
                    //_Back node will be deleted
                    _Back = _PrevL;
                }
            }

            _Size -= _Count;

            return (*this);
        }

        //this will disable the iterators
        FakeList &replace(const_iterator _Begin, const_iterator _End, const _Ty *_Newval, int _Newval_Len) {
            size_type _Pos = _PosList(_End._GetCurnode(), _End._GetCurpos()) - (_End - _Begin);

            erase(_Begin, _End);

            insert(_Newval, _Newval_Len, _Pos);

            return (*this);
        }

//        FakeList &replace(size_type _Begin, size_type _Count, const _Ty *_Newval, size_type _Newval_Len) {
//            node *_L, *_R;
//
//            _Count += _Begin;
//
//            _L = _Posnode(&_Begin);
//            _R = _Posnode(&_Count);
//
//            if (_Begin == _L->_Size) {
//                _Begin = 0;
//                _L = _L->_Next;
//            }
//            if (_Count == _R->_Size) {
//                _Count = 0;
//                _R = _R->_Next;
//            }
//
//            return replace(_L, _R, _Newval, _Newval_Len);
//        }

        iterator begin() NOEXCEPT{
            return iterator(_Front, 0);
        }

        const_iterator begin()const NOEXCEPT{
            return const_iterator(_Front, 0);
        }

        iterator end() NOEXCEPT{
            return iterator(NULL, 0);
        }

        const_iterator end() const NOEXCEPT{
            return const_iterator(NULL, 0);
        }

        _Ty &front() {
#if(DEBUG & DEBUG_RANGE_CHECK)
            if (_Back == NULL)
                throw std::out_of_range("FakeList");
#endif
            return (*_Front)[0];
        }

        const _Ty &front() const {
#if(DEBUG & DEBUG_RANGE_CHECK)
            if (_Back == NULL)
                throw std::out_of_range("FakeList");
#endif
            return (*_Front)[0];
        }

        _Ty &back() {
#if(DEBUG & DEBUG_RANGE_CHECK)
            if (_Back == NULL)
                throw std::out_of_range("FakeList");
#endif

            return (*_Back)[_Back->_Size - 1];
        }

        const _Ty &back() const {
#if(DEBUG & DEBUG_RANGE_CHECK)
            if (_Back == NULL)
                throw std::out_of_range("FakeList");
#endif
            return (*_Back)[_Back->_Size - 1];
        }

        //FakeList sublist(size_type begin, size_type end) {
        //	return (*this);
        //}

        //FakeList sublist(iterator begin, iterator end) {
        //	return (*this);
        //}

        /*iterator find()const {
            return end();
            }*/

        //DO NOT use it if not necessary
        FakeList clone(size_type _Max_size_of_each_node = DEFAULT_SIZE_OF_EACH_NODE)const {
            FakeList _Ret;

            if (this->_Front) {
                size_type _Num = this->_Size / _Max_size_of_each_node + (this->_Size%_Max_size_of_each_node ? 1 : 0);//nodes needed
                size_type _Len;
                size_type _Rest = this->_Size;

                _Len = (_Rest >= _Max_size_of_each_node ? _Max_size_of_each_node : _Rest);
                _Ret._Front = new node(new char[_Len], _Len);
                _Ret._Back = _Ret._Front;
                _Rest -= _Len;

                while (--_Num) {
                    _Len = (_Rest >= _Max_size_of_each_node ? _Max_size_of_each_node : _Rest);
                    _Ret._Back->_Next = new node(new char[_Len], _Len);
                    _Ret._Back = _Ret._Back->_Next;

                    _Rest -= _Len;
                }

                iterator _Des = _Ret.begin();
                for (const_iterator _Src = this->begin(); _Src != this->end(); ++_Src, ++_Des) {
                    *_Des = *_Src;
                }

                _Ret._Size = this->_Size;
            }
            return _Ret;
        }

        void swap(FakeList &_FakeList) {
            std::swap(_Size, _FakeList._Size);
            std::swap(_Front, _FakeList._Front);
            std::swap(_Back, _FakeList._Back);
        }

        void format(size_type _Max_size_of_each_node = DEFAULT_SIZE_OF_EACH_NODE) {
            *this = this->clone(_Max_size_of_each_node);
        }

        size_type size() const {
            return _Size;
        }

        size_type length() const {
            return _Size;
        }

        size_type list_node_length()const {
            size_type _Count = 0;
            node *_Node = _Front;
            while (_Node != NULL) {
                ++_Count;
                _Node = _Node->_Next;
            }
            return _Count;
        }

        bool empty() const {
            return _Size == 0;
        }

        void clear() {
            _Tidy(_Front);

            _Size = 0;
            _Front = NULL;
            _Back = NULL;
        }

        void _Tidy() {
            _Size = 0;
            _Front = NULL;
            _Back = NULL;
        }

        void _Tidy(node *_First, node *_End = NULL) {
            //delete nods [_First,_End)
            node *_Tmp;
            while (_First != _End) {
                _Tmp = _First->_Next;

                delete _First;

                _First = _Tmp;
            }
        }

        size_type _Tidy_n(node *_First, node *_End = NULL) {
            //delete nods [_First,_End) and return number of elements deleted
            size_type _Count = 0;

            node *_Tmp;
            while (_First != _End) {
                _Tmp = _First->_Next;

                _Count += _First->_Size;
                delete _First;

                _First = _Tmp;
            }

            return _Count;
        }

        void _Insert(_Ty *_Elem, size_type _Count, node *_Node, size_type _Pos) {
            node *_Insert_node = new node(_Elem, _Count);
            if (_Node->_Size == _Pos) {
                //insert to the end of the node
                _Insert_node->_Next = _Node->_Next;
                _Node->_Next = _Insert_node;
            }
            else if (_Pos == 0) {
                node *_Prev_node = _Prevnode(_Node);
                if (_Prev_node) {
                    _Prev_node->_Next = _Insert_node;
                }
                else {
                    _Front = _Insert_node;
                }
                _Insert_node->_Next = _Node;
            }
            else {
                //insert to mid of the node,disable the iterator
                //redesign the iterator if possible
                node *_Sep_node = new node(*_Node);

                _Sep_node->_Offset += _Pos;
                _Sep_node->_Size = _Node->_Size - _Pos;
                _Node->_Size = _Pos;

                _Sep_node->_Next = _Node->_Next;
                _Node->_Next = _Insert_node;
                _Insert_node->_Next = _Sep_node;
            }
        }

        node *_Prevnode(node *_Node) {
            //return NULL if _Node is the head
            node *_Tmp = _Front;
            while (_Tmp != NULL &&_Tmp->_Next != _Node) {
                _Tmp = _Tmp->_Next;
            }
            return _Tmp;
        }

        node *_Posnode(size_type *_Pos) {
            //find the node contains _Pos-th element,it never points to the beginning of a node
            node *_Node = _Front;
            while (_Node != NULL) {
                if (_Node->_Size >= *_Pos) {
                    break;
                }
                *_Pos -= _Node->_Size;
                _Node = _Node->_Next;
            }
            return _Node;
        }

        size_type _PosList(node *_Node, size_type _Pos) {
            node *_Tmp = _Front;
            while (_Tmp != _Node) {
                _Pos += _Tmp->_Size;
                _Tmp = _Tmp->_Next;
            }
            return _Pos;
        }

        _Ty *_Clone(const _Ty *_Elem, size_type _Count) {
            _Ty *_NewData = new _Ty[_Count];
            _Ty *_pData = _NewData;
            for (size_type i = 0; i < _Count; ++i, ++_pData, ++_Elem)
                *_pData = *_Elem;//mark

            return _NewData;
        }

        ~FakeList() {
            this->clear();
        }
    protected:
        size_type _Size;
        node *_Front;
        node *_Back;
    };


//CLASS string_builder
    class string_builder :public FakeList<char> {
    public:
        typedef FakeList<char> base;

        string_builder()
                :base() {
        }

        string_builder(const char *_String)
                :base(_String, strlen(_String)) {
        }

        string_builder(char *&&_String)
                :base(std::move(_String), strlen(_String)) {
        }

        string_builder(const char *_String, size_type _Count)
                : base(_String, _Count) {
        }

        string_builder(char *&&_String, size_type _Count)
                : base(std::move(_String), _Count) {
        }

        string_builder(string_builder &&_Str_builder)
                :base(std::move(_Str_builder)) {
        }

        string_builder(const string_builder &_Str_builder)
                :base(_Str_builder) {
        }

        string_builder &operator=(string_builder &&_Right) {
            base::operator=(std::move(_Right));

            return (*this);
        }

        string_builder &operator=(const string_builder &_Right) {
            base::operator=(_Right);

            return (*this);
        }

        string_builder &operator+=(const char *_Right) {
            FakeList::push_back(_Right, strlen(_Right));

            return (*this);
        }

        string_builder &operator+=(char *&&_Right) {
            FakeList::push_back(std::move(_Right), strlen(_Right));

            return (*this);
        }

        string_builder &operator+=(string_builder &&_Right) {
            FakeList::push_back(std::move(_Right));

            return (*this);
        }

        string_builder &push_front(const char *_String) {
            base::push_front(_String, strlen(_String));

            return (*this);
        }

        string_builder &push_front(char *&&_String) {
            base::push_front(std::move(_String), strlen(_String));

            return (*this);
        }

        string_builder &push_front(const char *_String, size_type _Count) {
            base::push_front(_String, _Count);

            return (*this);
        }

        string_builder &push_front(char *&&_String, size_type _Count) {
            base::push_front(std::move(_String), _Count);

            return (*this);
        }

        string_builder &push_front(char _C) {
            base::push_front(_C);

            return (*this);
        }

        string_builder &push_front(string_builder &&_Str_builder) {
            base::push_front(std::move(_Str_builder));

            return (*this);
        }

        string_builder &push_back(const char *_String) {
            base::push_back(_String, strlen(_String));

            return (*this);
        }

        string_builder &push_back(char *&&_String) {
            base::push_back(std::move(_String), strlen(_String));

            return (*this);
        }

        string_builder &push_back(const char *_String, size_type _Count) {
            base::push_back(_String, _Count);

            return (*this);
        }

        string_builder &push_back(char *&&_String, size_type _Count) {
            base::push_back(std::move(_String), _Count);

            return (*this);
        }

        string_builder &push_back(char _C) {
            base::push_back(_C);

            return (*this);
        }

        string_builder &push_back(string_builder &&_Str_builder) {
            base::push_back(std::move(_Str_builder));

            return (*this);
        }

        std::string to_string() const {
            std::string _Str;

            _Str.resize(_Size + 1);
            _Str.clear();

            node *_Node = _Front;
            while (_Node != NULL) {
                _Str.append(&(*_Node)[0], _Node->_Size);
                _Node = _Node->_Next;
            }

            return _Str;
        }

        string_builder clone(size_type _Max_size_of_each_node = DEFAULT_SIZE_OF_EACH_NODE_CHAR) const {
            string_builder _Ret;

            base *pRet = &_Ret;
            pRet->assign(base::clone(_Max_size_of_each_node));

            return _Ret;
        }

        string_builder &format(size_type _Max_size_of_each_node = DEFAULT_SIZE_OF_EACH_NODE_CHAR) {
            base::format(_Max_size_of_each_node);

            return (*this);
        }

        void print(bool _Add_LF = false) const {

#if(DEBUG & DEBUG_PRINT_NODE)
            printf("<");
#endif

            for (const_iterator _Iter = this->begin(); _Iter != this->end(); ++_Iter)
                printf("%c", *_Iter);

#if(DEBUG & DEBUG_PRINT_NODE)
            printf("||%d>", _Size);
#endif

            if (_Add_LF)printf("\n");
        }

        /*string_builder substr() const {
            return string_builder();
            }*/

    protected:
        //	static char *_Clone(const char *_String, size_type _Count) {
        //		char *_New_string = new char[_Count];
        //		memcpy(_New_string, _String, sizeof(char)*_Count);
        //
        //		return _New_string;
        //	}
    };

    inline string_builder operator+(const char *_String, string_builder &&_String_builder) {
        string_builder _Ret = _String;

        _Ret += std::move(_String_builder);

        return _Ret;
    }

_NINI_END

#endif //_FAKELIST_ABACKER_NINI_H_