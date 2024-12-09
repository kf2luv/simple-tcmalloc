#pragma once
#include "thread-cache/ThreadCache.hh"
#include <thread>


thread_local ThreadCache* pTLSThreadCache = nullptr;

void* ccAlloc(size_t size){
    if(pTLSThreadCache == nullptr){
        pTLSThreadCache = new ThreadCache;
    }
    std::cout << std::this_thread::get_id() << ": " << pTLSThreadCache << std::endl;
    return pTLSThreadCache->Allocate(size);
}

void ccFree(void* obj){
    
}