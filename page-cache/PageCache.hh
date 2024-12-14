#include "../common/common.hh"

namespace cc_memory_pool
{
    class PageCache
    {
    public:
        static PageCache *getInstance();

        // 根据指定的页数，创建一个新的span
        Span *newSpan(size_t npage);

    private:
        PageCache() {
            
        }
        PageCache(const PageCache &other) = delete;
        PageCache &operator=(const PageCache &other) = delete;

        SpanList _spanLists[NPAGELISTS + 1]; // 直接映射，下标i表示：链表中每个span挂载i个page
        std::mutex _mtx;                     // PageCache整体的锁
        static PageCache _instance;
    };
}
