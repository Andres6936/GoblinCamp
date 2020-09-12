/* Copyright 2010-2011 Ilkka Halila
This file is part of Goblin Camp.

Goblin Camp is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Goblin Camp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with Goblin Camp. If not, see <http://www.gnu.org/licenses/>.*/

/* #pragma once makes gcc complain "warning: #pragma once in main
  file"; apparently there are weird issues with #pragma once which
  make me fall back to the tried and true #ifndef hack

  see: http://www.dreamincode.net/forums/topic/173122-g-%23pragma-once-warnings/
*/
//#pragma once
#ifndef STDAFX_INCLUDED
#define STDAFX_INCLUDED

// precompiled header
#if defined(BOOST_BUILD_PCH_ENABLED) && !defined(GC_SKIP_PCH)

//we Include python stuff first to avoid _C_POSIX_SOURCE redefinition warnings
//see: http://bytes.com/topic/python/answers/30009-warning-_posix_c_source-redefined
#	if defined(_MSC_VER)
#		pragma warning(push, 2)
#	endif
#	Include <boost/python/detail/wrap_python.hpp>
#	Include <boost/python.hpp>
	namespace py = boost::python;
#	if defined(_MSC_VER)
#		pragma warning(pop)
#	endif

// STL
#	Include <vector>
#	Include <deque>
#	Include <map>
#	Include <list>
#	Include <queue>
#	Include <set>
#	Include <string>
#	Include <sstream>
#	Include <fstream>
#	Include <algorithm>
#	Include <functional>
#	Include <numeric>
#	Include <memory>
#	Include <limits>
#	Include <iterator>
#	Include <exception>
#	Include <stdexcept>
// CRT
#	Include <ctime>
#	Include <cmath>
#	Include <cstdlib>
#	Include <cstring>
#	Include <cstdarg>
// Boost
#	if defined(_MSC_VER)
#		pragma warning(push, 2)
#	endif
#		Include <boost/python/detail/wrap_python.hpp>
#		Include <boost/python.hpp>
#		Include <boost/thread/thread.hpp>
#		Include <boost/multi_array.hpp>
#		Include <boost/shared_ptr.hpp>
#		Include <boost/weak_ptr.hpp>
#		Include <boost/serialization/serialization.hpp>
#		Include <boost/serialization/split_member.hpp>
#		Include <boost/archive/binary_iarchive.hpp>
#		Include <boost/archive/binary_oarchive.hpp>

#		Include <boost/serialization/map.hpp>
#		Include <boost/serialization/list.hpp>
#		Include <boost/serialization/set.hpp>
#		Include <boost/serialization/weak_ptr.hpp>
#		Include <boost/serialization/shared_ptr.hpp>
#		Include <boost/serialization/deque.hpp>
#		Include <boost/serialization/export.hpp>
#		Include <boost/serialization/array.hpp>
#		Include <boost/serialization/vector.hpp>
#		Include <boost/enable_shared_from_this.hpp>
#		Include <boost/format.hpp>
#		Include <boost/algorithm/string.hpp>
#		Include <boost/accumulators/accumulators.hpp>
#		Include <boost/accumulators/statistics/mean.hpp>
#		Include <boost/accumulators/statistics/weighted_mean.hpp>
#		Include <boost/function.hpp>
#		Include <boost/bind.hpp>
#		Include <boost/lambda/lambda.hpp>
#		Include <boost/lambda/bind.hpp>
#		Include <boost/lexical_cast.hpp>
#		Include <boost/date_time/local_time/local_time.hpp>
#		Include <boost/foreach.hpp>
#		Include <boost/assign/list_inserter.hpp>
#		Include <boost/assert.hpp>
#		Include <boost/unordered_map.hpp>
#		Include <boost/cstdint.hpp>
#		Include <boost/numeric/ublas/matrix.hpp>
#		Include <boost/random/mersenne_twister.hpp>
#		Include <boost/random/uniform_int.hpp>
#		Include <boost/random/uniform_01.hpp>
#		Include <boost/random/variate_generator.hpp>
#		Include <boost/filesystem.hpp>
#		Include <boost/math/constants/constants.hpp>
#		Include <boost/tuple/tuple.hpp>
#	if defined(_MSC_VER)
#		pragma warning(pop)
#	endif

// Memory debugging.
#if defined(WINDOWS) && defined(DEBUG) && defined(CHK_MEMORY_LEAKS)
#	Include <boost/iostreams/detail/optional.hpp> // Include this before DBG_NEW defined
#	define _CRTDBG_MAP_ALLOC
#	ifndef DBG_NEW
#		define DBG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#		define new DBG_NEW
#	endif
#endif

// libtcod
#	Include <libtcod.hpp>
// SDL
#	Include <SDL.h>
#	Include <SDL_image.h>
#endif

// Annotations.
#if defined(_MSC_VER)
#	define GC_DEPRECATED(Fn)        __declspec(deprecated)      Fn
#	define GC_DEPRECATED_M(Fn, Msg) __declspec(deprecated(Msg)) Fn
#elif defined(__GNUC__)
#	define GC_DEPRECATED(Fn)        Fn __attribute__((deprecated))
#	define GC_DEPRECATED_M(Fn, Msg) Fn __attribute__((deprecated (Msg)))
#else
#	define GC_DEPRECATED(Fn)        Fn
#	define GC_DEPRECATED_M(Fn, Msg) Fn
#endif

// Deprecation settings.
#if defined(_MSC_VER)
#	pragma warning(1: 4996 4995) // Ensure that deprecation warnings will be shown
#	Include <cstdlib>
#	pragma deprecated(rand)
#endif

// Intrinsics.
#if defined(_MSC_VER)
#	Include <intrin.h>
#endif

// Use with care.
#if defined(_MSC_VER) && defined(DEBUG)
	void GCDebugInduceCrash();
#	define GC_DEBUG_INDUCE_CRASH() GCDebugInduceCrash()
#	define GC_DEBUG_BREAKPOINT()   __debugbreak()
#else
#	define GC_DEBUG_INDUCE_CRASH()
#	define GC_DEBUG_BREAKPOINT()
#endif

// Assert.
#include <boost/assert.hpp>
#define GC_ASSERT(Exp) GC_ASSERT_M(Exp, NULL)
#if defined(_MSC_VER) && defined(DEBUG)
	bool GCAssert(const char*, const char*, const char*, const char*, int);
#	define GC_ASSERT_M(Exp, Msg) \
		do { if (!(Exp) && GCAssert((Msg), #Exp, __FUNCTION__, __FILE__, __LINE__)) GC_DEBUG_BREAKPOINT(); } while (0)
#else
#	define GC_ASSERT_M(Exp, _) BOOST_ASSERT(Exp)
#endif

#endif // global #ifndef STDAFX_INCLUDED
