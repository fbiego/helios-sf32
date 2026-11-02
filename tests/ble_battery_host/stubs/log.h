static inline void test_log(const char *format, ...) { (void)format; }
#define LOG_I(...) test_log(__VA_ARGS__)
#define LOG_W(...) test_log(__VA_ARGS__)
#define LOG_D(...) test_log(__VA_ARGS__)
