#pragma once
#include <cassert>
#include <iostream>

class FreeList {
   public:
    void push(void* obj) {  // 头插
        assert(obj != nullptr);
        nextObj(obj) = _free_list;
        _free_list = obj;
    }
    void pushRange(void* begin, void* end) {
        assert(begin && end);
        nextObj(end) = _free_list;
        _free_list = begin;
    }

    void* pop() {
        if (_free_list == nullptr) {
            return nullptr;
        }
        void* ret = _free_list;
        _free_list = nextObj(_free_list);
        return ret;
    }
    size_t popNum(void* begin, void* end, size_t n) { //pop n个节点，如果不够，就全部pop出去
        void* left = _free_list;
        void* right = left;

        int cnt = 1;
        while(nextObj(right) != nullptr && cnt < n){
            right = nextObj(right);
            cnt++;
        }
        _free_list = nextObj(right);
        nextObj(right) = nullptr;

        begin = left;
        end = right;
        return cnt;
    }
    
    size_t& maxFetchNum(){ //能从中央缓存获取的最大obj数量
        return _maxFetchNum;
    }
    bool empty() { return _free_list == nullptr; }

   private:
    void*& nextObj(void* obj) {  // 获取内存对象的下一个对象的地址
        return *(void**)obj;
    }

    void* _free_list = nullptr;
    size_t _maxFetchNum = 1;
};