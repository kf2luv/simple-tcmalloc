#pragma once
#include <iostream>
#include <cassert>
#include <mutex>

// 获取bytes的最大值
static const size_t MAX_MEM_SIZE = 256 * 1024;
// ThreadCache从CentralCache拿取obj个数最大值 (32768)
static const size_t MAX_FETCH_NUM = MAX_MEM_SIZE / 8;
//自由链表最大个数（thread cache中）
static const size_t NFREELISTS = 208;
//span链表最大个数 (central cache中)
static const size_t NSPANLISTS = 208;
//page链表最大个数 (page cache中)
static const size_t NPAGELISTS = 128;
//规定一页8KB (8KB = 2^13B)
static const size_t PAGE_SHIFT = 13;


#include "SizeClass.hh"
#include "FreeList.hh"
#include "Span.hh"




