test: test.cc thread-cache/ThreadCache.cc central-cache/CentralCache.cc page-cache/PageCache.cc common/*.cc
	g++ -o $@ $^ -lpthread -std=c++11
.PHONY:
clean:
	rm test -f