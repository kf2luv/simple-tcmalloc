test: test.cc thread-cache/ThreadCache.cc
	g++ -o $@ $^ -lpthread

.PHONY:
clean:
	rm test -f