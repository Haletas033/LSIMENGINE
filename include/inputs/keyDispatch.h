#define GET_NARGS(_1,_2,N,...) N
#define VA_NARGS(...) GET_NARGS(__VA_ARGS__, 2, 1)
#define DISPATCHER(MACRO, ...) MACRO(__VA_ARGS__)
#define CONCAT(a, b) a##b
#define CONCAT_(a, b) CONCAT(a, b)
#define K(...) DISPATCHER(CONCAT_(K, VA_NARGS(__VA_ARGS__)), __VA_ARGS__)