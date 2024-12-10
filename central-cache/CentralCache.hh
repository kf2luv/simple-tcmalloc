#pragma once
#include "../common/common.hh"

//单例模式（饿汉模式）
class CentralCache {
   public:
    static CentralCache* getInstance();

    //从CentralCache里由bytes映射的桶SpanList中，获取fetchNum个obj，将结果保存到[begin, end]
    //返回获取到的obj实际数量
    size_t getRangeObj(void* begin, void* end, size_t fetch_num, size_t bytes);

    Span* getOneSpan(size_t idx);

   private:
    CentralCache() {};
    CentralCache(const CentralCache& other) = delete;
    bool operator=(const CentralCache& other) = delete;

    SpanList _span_lists[NSPANLISTS];
    static CentralCache _instance;
};