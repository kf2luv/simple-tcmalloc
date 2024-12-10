test: test.cc thread-cache/ThreadCache.cc central-cache/CentralCache.cc
	g++ -o $@ $^ -lpthread

.PHONY:
clean:
	rm test -f