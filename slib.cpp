
#ifdef _WIN32
#include <metric_win.cpp>
#elif __APPLE__
#include <metric_mac.cpp>
#endif
#include <metric.cpp>
#include <profiler.cpp>
