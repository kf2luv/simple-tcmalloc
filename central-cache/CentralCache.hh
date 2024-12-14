#pragma once
#include "../common/common.hh"
#include "../page-cache/PageCache.hh"

// 单例模式（饿汉模式
namespace cc_memory_pool
{
    class CentralCache
    {
    public:
        static CentralCache *getInstance();

        // 从CentralCache里由bytes映射的桶SpanList中，获取fetchNum个obj，将结果保存到[begin, end]
        // 返回获取到的obj实际数量
        size_t getRangeObj(void *begin, void *end, size_t fetchNum, size_t bytes);

        // 从指定spanList中，获取一个有效的span
        Span *getOneEffectiveSpan(SpanList &spanList, size_t bytes);

    private:
        CentralCache() {}
        CentralCache(const CentralCache &other) = delete;
        CentralCache &operator=(const CentralCache &other) = delete;

        SpanList _spanLists[NSPANLISTS];
        static CentralCache _instance;
    };
}
