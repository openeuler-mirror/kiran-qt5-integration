/**
 * Copyright (c) 2023 ~ 2024 KylinSec Co., Ltd.
 * kiran-qt5-integration is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     tangjie02 <tangjie02@kylinsec.com.cn>
 */

#pragma once

#include <cstdio>
#include <functional>
#include <vector>

#define CONNECTION(text1, text2) text1##text2
#define CONNECT(text1, text2) CONNECTION(text1, text2)

class KQIDefer
{
public:
    KQIDefer(std::function<void(std::string)> func, std::string fun_name) : func_(func),
                                                                            fun_name_(fun_name) {}
    ~KQIDefer() { func_(fun_name_); }

private:
    std::function<void(std::string)> func_;
    std::string fun_name_;
};

// helper macro for KQIDefer class
#define SCOPE_EXIT(block) KQIDefer CONNECT(_defer_, __LINE__)([&](std::string _arg_function) block, __FUNCTION__)

#define BREAK_IF_FALSE(cond) \
    {                        \
        if (!(cond)) break;  \
    }

#define BREAK_IF_TRUE(cond) \
    {                       \
        if (cond) break;    \
    }

#define RETURN_VAL_IF_FALSE(cond, val) \
    {                                  \
        if (!(cond))                   \
        {                              \
            return val;                \
        }                              \
    }

#define RETURN_VAL_IF_TRUE(cond, val) \
    {                                 \
        if (cond) return val;         \
    }

#define RETURN_IF_FALSE(cond) \
    {                         \
        if (!(cond))          \
        {                     \
            return;           \
        }                     \
    }

#define RETURN_IF_TRUE(cond) \
    {                        \
        if (cond) return;    \
    }

#define CONTINUE_IF_FALSE(cond) \
    {                           \
        if (!(cond)) continue;  \
    }

#define CONTINUE_IF_TRUE(cond) \
    {                          \
        if (cond) continue;    \
    }

#define IGNORE_EXCEPTION(expr)          \
    {                                   \
        try                             \
        {                               \
            expr;                       \
        }                               \
        catch (const Glib::Error &e)    \
        {                               \
        }                               \
        catch (const std::exception &e) \
        {                               \
        }                               \
    }

#define POINTER_TO_STRING(p) ((p) ? QString(p) : QString())

using StringHash = uint32_t;

constexpr StringHash prime = 9973;
constexpr StringHash basis = 0xCBF29CE4ul;
constexpr StringHash hash_compile_time(char const *str, StringHash last_value = basis)
{
    return *str ? hash_compile_time(str + 1, (StringHash)((*str ^ last_value) * (uint64_t)prime)) : last_value;
}

inline StringHash shash(char const *str)
{
    StringHash ret{basis};

    while (*str)
    {
        ret ^= *str;
        ret *= prime;
        str++;
    }

    return ret;
}

/// compile-time hash of string.
/// usage: "XXX"_hash
constexpr StringHash operator"" _hash(char const *p, size_t)
{
    return hash_compile_time(p);
}