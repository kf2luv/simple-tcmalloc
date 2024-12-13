#pragma once

class FreeList
{
public:
    // 给一个自由链表的头结点，创建FreeList对象
    FreeList(void *head = nullptr)
        : _freeList(head), _maxFetchNum(1)
    {
    }

    void push(void *obj)
    { // 头插
        assert(obj != nullptr);
        nextObj(obj) = _freeList;
        _freeList = obj;
    }
    void pushRange(void *begin, void *end)
    {
        assert(begin && end);
        nextObj(end) = _freeList;
        _freeList = begin;
    }

    void *pop()
    {
        if (_freeList == nullptr)
        {
            return nullptr;
        }
        void *ret = _freeList;
        _freeList = nextObj(_freeList);
        return ret;
    }
    size_t popNum(void *begin, void *end, size_t n)
    { // pop n个节点，如果不够，就全部pop出去
        void *left = _freeList;
        void *right = left;

        int cnt = 1;
        while (nextObj(right) != nullptr && cnt < n)
        {
            right = nextObj(right);
            cnt++;
        }
        _freeList = nextObj(right);
        nextObj(right) = nullptr;

        begin = left;
        end = right;
        return cnt;
    }

    size_t &maxFetchNum()
    { // 能从中央缓存获取的最大obj数量
        return _maxFetchNum;
    }
    bool empty() { return _freeList == nullptr; }

public:
    static void *&nextObj(void *obj)
    { // 获取内存对象的下一个对象的地址
        return *(void **)obj;
    }

private:
    void *_freeList;
    size_t _maxFetchNum;
};