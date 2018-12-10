#pragma once

extern void trace(const char *file, const char *function, int line);

#define TRACE trace(__FILE__, __FUNCTION__, __LINE__)
