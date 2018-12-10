#pragma once

[[noreturn]] extern void panic(const char *message, const char *file,
                               const char *function, int line);

#define PANIC(message) panic(message, __FILE__, __FUNCTION__, __LINE__)

#define UNEXPECTED() PANIC("unexpected")
