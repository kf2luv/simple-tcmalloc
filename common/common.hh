#pragma once
#include <cassert>
#include <iostream>
#include <mutex>

// 获取bytes的最大值
static const size_t MAX_MEM_SIZE = 256 * 1024;
// ThreadCache从CentralCache拿取obj个数最大值
static const size_t MAX_FETCH_NUM = MAX_MEM_SIZE / 8;

static const size_t NFREELISTS = 208;
static const size_t NSPANLISTS = 208;

#include "SizeClass.hh"
#include "FreeList.hh"
#include "Span.hh"




