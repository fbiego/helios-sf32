#ifndef STORAGE_TEST_LOG_H
#define STORAGE_TEST_LOG_H
static inline void test_log(const char *format, ...) { (void)format; }
#define LOG_I(...) test_log(__VA_ARGS__)
#define LOG_W(...) test_log(__VA_ARGS__)
#define LOG_E(...) test_log(__VA_ARGS__)
#endif
