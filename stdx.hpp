#ifndef STANDARD_EXTENDED_HPP
#define STANDARD_EXTENDED_HPP

// Macros

// Note: check https://blogs.msdn.microsoft.com/vcblog/2018/04/09/intellisense-for-remote-linux-headers/?utm_source=vs_developer_news&utm_medium=referral

#if defined(STDX_INCLUDE_MACROS)
#include "macros.h"
#endif

// Types

#include "comparable.hpp"
#include "generative_iterator.hpp"
#include "random_generator.hpp"
#include "spin_mutex.hpp"

// Methods

#include "binder.hpp"
#include "zeromem.hpp"

#endif//STANDARD_EXTENDED_HPP
