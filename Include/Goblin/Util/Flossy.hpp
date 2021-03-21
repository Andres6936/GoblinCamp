/*
    flossy 1.0

    This project is free software; you can redistribute it and/or modify it
    under the terms of the MIT license:

    Copyright (c) 2016 Florian Kesseler

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/


/*
  The full documentation for the public API is at the very end of this file.
  Here is a summary:


1. Generic formatting function using iterators:

  OutIt format_it(OutIt out, InputIt start, InputIt const end, 
                  FirstValueT const& first, ValueTs... elements)
 
  This function is the main work horse of flossy. It does all the format
  string evaluation and formatting of elements.
  It recursively calls itself (or the "no values remain" overload above) to
  piecewise construct the output string.
 
  Parameters:
    out          Output iterator to store the resulting string characters.
    start        Iterator to beginning of format string.
    end          Iterator to the character one element after the last
                 character of the format string.
    first        The first value to be converted.
    elements     Remaining values to be used in later conversions.
 
  Usage example:
 
    std::string result;
    std::string format_str("The first value passed is {}"
                           ", and the second is {}!");
    auto it = format_it(
      std::back_inserter(result), format_str.begin(), format_str.end(), 
      42, "foo");



2. Function to format a string directly
  
  std::basic_string<CharT> format(std::basic_string<CharT> const & format_str,
                                  ValueTs... elements)

  Convenience function wrapper for format_it that allows formatting a format
  string and values directly into a string and returning that.
 
  Parameters:
    format_str  Format string to be used when formatting the string. It will
                be passed to format_it directly.
    elements    The elements to be formatted. They are passed to format_it
                verbatim.
 
  Usage example:
 
    auto result = format("The first value passed is {}, and the second is {}!"s,
                         42, "foo");
 

3. Function to format a string to a stream

  Convenience function wrapper for format_it that allows formatting a format
  string and values directly to an ostream:

  std::basic_ostream<CharT, Traits>& format(
    std::basic_ostream<CharT, Traits> &ostream,
    std::basic_string<CharT> const& format_str,
    ValueTs... elements)
 
  Parameters:
    ostream     Stream to write the resulting string to.
    format_str  Format string to be used when formatting the string. It will
                be passed to format_it directly.
    elements    The elements to be formatted. They are passed to format_it
                verbatim.
 
  Usage example:
 
    format(std::cout, "The first value passed is {}, and the second is {}!"s,
           42, "foo");
 

4. Format Strings

  Format strings consist of any character sequence containing any number of
  conversion specifiers.

  Conversion specifiers are delimited by curly braces containing the format
  specification:

  format: [align][sign][0][width][.precision][type]
  align: '>' | '_' | '<'
  sign: '+' | ' ' | '-'
  width: integer
  precision: integer
  type: 'd', 'o', 'x', 'f', 'e', 's', 'b'

  'align' specifies where in the resulting field the value will be aligned, as
  described in the following table:

  Option   Description
       >   Align the value to the right of the field (i.e. put spaces left of
           the resulting value). If this option is used, the zero flag is
           ignored. This is the default option.
       <   Align the value to the left of the field (i.e put spaces right of
           the resulting value.  If this option is used, the zero flag is
           ignored.
       _   Let the value fill the whole field by putting spaces or zeroes
           between the digits and the sign.  This is the only option that
           respects the zero flag.


  'sign' specifies how positive numbers should be displayed, as described in
  the following table:

  Option   Description
       -   Only display the sign of negative numbers. This is the default
           option.
       +   Positive numbers will be displayed with a plus sign ('+') in front
           of them.
 <space>   Positive numbers will be displayed with a space in front of them to
           align them with negative numbers.

 
  The 'zero' flag ('0'), if present, makes numbers displayed with internal
  alignment (the '_' align option) use a zero ('0') character instead of spaces
  to fill the gap between numbers and sign.

  'width' specifies the minimum width of the field. If this is larger than the
  width of the converted value, the value will be aligned and padded according
  to the 'align' and 'zero' flags.

  'precision' specifies the number of digits in the fractional part of floating
  point numbers.

  'type' specifies the formatting method used. This is basically used to change
  the display type of numbers, like the number base or float representation
  (scientific vs. fixed width) and is ignored if it doesn't make sense for the
  field currently converted.

  The values have the same meaning as in printf, with the addition of 'b',
  which outputs an integer in binary form.


5. User Defined Types

  You may add your own formatting function for any type you want. To do
  that, you have to provide a templated function 'format_element', taking at
  least two template parameters and exactly three function parameters:

  template<typename CharT, typename OutIt>
  OutIt format_element(OutIt out, conversion_options options, YourType value);

  Mandatory template parameters:
    CharT     Character type to generate.
    OutIt     Output iterator type used by the format function.

    You may add more template parameters if you wish, as long as they can
    always be deduced from the type you want to convert.

  Parameters:
    out       The output iterator your format function should write to
    options   The options selected in the format specifier (see section 4).
    value     The value you passed in the format or format_it function.

  Return value:
    The updated `out` iterator.

  You may use existing flossy::format_element functions for integers, floats,
  characters, and strings to built upon.

  conversion_options is holds the format flags specified in the format string.
  See definition of conversion_options for more information.

  There is currently no way to add your own conversion flags or options, sorry.
*/


#ifndef FLOSSY_H_INCLUDED
#define FLOSSY_H_INCLUDED


#ifndef FLOSSY_FLOAT_METHOD
# define FLOSSY_FLOAT_METHOD FLOSSY_FLOAT_METHOD_SSTREAM
#endif

#define FLOSSY_FLOAT_METHOD_SSTREAM 0
#define FLOSSY_FLOAT_METHOD_FAST    1 // not implemented, yet
#define FLOSSY_FLOAT_METHOD_GRISU   2 // not implemented, yet

#include <string_view>
#include <exception>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <cstdint>
#include <limits>
#include <vector>
#include <cmath>
#include <array>

namespace flossy
{

	/**
	 * Function used for the client of library
	 * @return Version of Flossy
	 */
	[[maybe_unused]] constexpr float get_version() noexcept
	{
		return 2021.1f;
	}

	namespace internal
	{

		// Used only for types that allow different representations, i.e. not for
		// strings.
		enum class conversion_format
		{
			binary,
			decimal,
			octal,
			hex,
			normal_float,
			scientific_float,
			normal,
			string,
			character,
			fail
		};


		// Where to put zeroes and spaces when filling up a field to width.
		enum class fill_alignment
		{
			left,
			intern,
			right
		};


		// How to display the sign of positive numbers
		enum class pos_sign_type
		{
			plus,
			space,
			none
		};


		struct conversion_options
		{
			conversion_format format = conversion_format::normal;
			int width = 0;
			int precision = 6;
			fill_alignment alignment = fill_alignment::left;
			pos_sign_type pos_sign = pos_sign_type::none;
			bool zero_fill = false;

			conversion_options(
					conversion_format format = conversion_format::normal, int width = 0,
					int precision = 6,
					fill_alignment align = fill_alignment::left,
					pos_sign_type pos_sign = pos_sign_type::none,
					bool zero_fill = false)
					: format(format), width(width), precision(precision), alignment(align),
					  pos_sign(pos_sign),
					  zero_fill(zero_fill)
			{
			}
		};

		template<typename InputIt>
		inline void ensure_not_equal(InputIt const& a, InputIt const& b)
		{
			if (a == b)
			{
				throw std::invalid_argument("unterminated {");
			}
		}

		// Helper class to parse the conversion options
		template<typename InputIt>
		class option_reader
		{
			typedef typename std::iterator_traits<InputIt>::value_type char_type;

			// Observe the initialization of member variables.
			// Reference: https://stackoverflow.com/a/27669457

			const std::array<std::pair<char_type, fill_alignment>, 3> alignment_types{{
																							  { '>', fill_alignment::left },
																							  { '_', fill_alignment::intern },
																							  { '<', fill_alignment::right }
																					  }};

			const std::array<std::pair<char_type, pos_sign_type>, 3> sign_types{{
																						{ '+', pos_sign_type::plus },
																						{ ' ', pos_sign_type::space },
																						{ '-', pos_sign_type::none }
																				}};

			const std::array<std::pair<char_type, conversion_format>, 8> format_types{{
																							  { 'b', conversion_format::binary },
																							  { 'd', conversion_format::decimal },
																							  { 'o', conversion_format::octal },
																							  { 'x', conversion_format::hex },
																							  { 'e', conversion_format::scientific_float },
																							  { 'f', conversion_format::normal_float },
																							  { 's', conversion_format::string },
																							  { 'c', conversion_format::character }
																					  }};

			InputIt& it;
			InputIt const end;

		public:
			conversion_options options;

			inline option_reader(InputIt& start, InputIt const end)
					: it(start), end(end)
			{
				read_options();
			}


			// Ensure the input iterator is not at the end of input.
			inline void check_it() const
			{
				ensure_not_equal(it, end);
			}


			// Read a character from the input iterator, map it to one of the given values.
			template<typename ValueT, std::size_t Number>
			inline void
			map_char(std::array<std::pair<char_type, ValueT>, Number> const& values, ValueT& out)
			{
				check_it();
				auto const c = *it;
				auto v = std::find_if(values.begin(), values.end(), [=](auto const& a)
				{ return a.first == c; });
				if (v != values.end())
				{
					out = v->second;
					++it;
				}
			}


			// Read alignment of field
			inline void read_align()
			{
				map_char(alignment_types, options.alignment);
			}


			// Read zero-fill field
			inline void read_fill()
			{
				check_it();
				if (*it == '0')
				{
					++it;
					options.zero_fill = true;
				}
			}


			// Read positive sign flag (none, space or plus)
			inline void read_sign()
			{
				map_char(sign_types, options.pos_sign);
			}


			inline int read_number()
			{
				int v = 0;
				for (;;)
				{
					check_it();
					auto const c = *it;
					if (c >= '0' && c <= '9')
					{
						v = v * 10 + (c - '0');
						++it;
					}
					else
					{
						return v;
					}
				}
			}


			inline void read_width()
			{
				options.width = read_number();
			}


			inline void read_precision()
			{
				check_it();
				if (*it == '.')
				{
					++it;
					options.precision = read_number();
				}
			}


			inline void read_format()
			{
				map_char(format_types, options.format);
			}


			inline void read_options()
			{
				read_align();
				read_sign();
				read_fill();
				read_width();
				read_precision();
				read_format();

				check_it();
				if (*it != '}')
				{
					throw std::invalid_argument("Invalid character in format string");
				}
				++it;
			}
		};


		// Output string with space padding on the appropriate side
		template<typename CharT, typename OutIt, typename InputIt>
		OutIt
		format_string(OutIt out, conversion_options const& options, InputIt start, InputIt end)
		{
			int fill_count = options.width - (end - start);
			if (fill_count < 0)
			{
				fill_count = 0;
			}

			if (options.alignment == fill_alignment::left)
			{
				out = std::fill_n(out, fill_count, CharT(' '));
				out = std::copy(start, end, out);
			}
			else
			{
				out = std::copy(start, end, out);
				out = std::fill_n(out, fill_count, CharT(' '));
			}

			return out;
		}


		// Digits for integer conversions
		template<typename CharT>
		constexpr CharT digit_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a',
											'b', 'c', 'd', 'e',
											'f' };


		// Convert format flag to number system base
		template<typename ValueT>
		constexpr ValueT int_format_radix(conversion_format format)
		{
			switch (format)
			{
			case conversion_format::hex:
				return ValueT(16);
			case conversion_format::octal:
				return ValueT(8);
			case conversion_format::binary:
				return ValueT(2);
			default:
				return ValueT(10);
			}
		}


		// Holds the characters of a string with the appropriate size for all numbers
		template<typename CharT>
		struct digit_buffer
		{
			// This should be enough space for all integer types supported by the
			// compiler, in binary representation and thus for all integer types in all
			// bases.
			std::array<CharT, std::numeric_limits<uintmax_t>::digits> digits;
			int count = 0;


			// Insert character into buffer.
			void insert(CharT c)
			{
				digits[count++] = c;
			}


			// Copy the accumulated characters to the output iterator
			template<typename OutIt>
			OutIt output(OutIt out) const
			{
				for (int i = count; i > 0; --i)
				{
					*out++ = digits[i - 1];
				}
				return out;
			}
		};


		// Generate the digit characters for the given unsigned value
		template<typename CharT, typename ValueT>
		digit_buffer<CharT> generate_digits(ValueT value, conversion_format const& format)
		{
			static_assert(std::is_unsigned<ValueT>::value,
					"ValueT must be unsigned in generate_digits");

			digit_buffer<CharT> digits;

			const ValueT radix = int_format_radix<ValueT>(format);

			do
			{
				digits.insert(digit_chars<CharT>[value % radix]);
				value /= radix;
			} while (value);

			return digits;
		}


		// The sign character to output when formatting a number
		enum class sign_character
		{
			none,
			space,
			plus,
			minus
		};


		// Output the given sign to the iterator
		template<typename CharT, typename OutIt>
		OutIt output_sign(OutIt out, sign_character sign)
		{
			if (sign == sign_character::space)
			{
				*out++ = CharT(' ');
			}
			else if (sign == sign_character::plus)
			{
				*out++ = CharT('+');
			}
			else if (sign == sign_character::minus)
			{
				*out++ = CharT('-');
			}

			return out;
		}


		// Output values given by out_func to the output iterator and add padding and
		// sign characters.
		template<typename CharT, typename OutIt, typename DigitOutFunc>
		OutIt output_padded_with_sign(
				OutIt out, DigitOutFunc out_func, int digit_count,
				conversion_options const& options,
				sign_character sign)
		{
			int fill_count = options.width - digit_count - (sign == sign_character::none ? 0 : 1);

			if (fill_count < 0)
			{
				fill_count = 0;
			}

			const auto fill = options.zero_fill ? CharT('0') : CharT(' ');

			if (options.alignment == fill_alignment::left)
			{
				out = std::fill_n(out, fill_count, fill);
				out = output_sign<CharT>(out, sign);
				out = out_func();
			}
			else if (options.alignment == fill_alignment::intern)
			{
				out = output_sign<CharT>(out, sign);
				out = std::fill_n(out, fill_count, fill);
				out = out_func();
			}
			else if (options.alignment == fill_alignment::right)
			{
				out = output_sign<CharT>(out, sign);
				out = out_func();
				out = std::fill_n(out, fill_count, fill);
			}

			return out;
		}


		// Format a decomposed integer with fill characters and sign
		template<typename OutIt, typename CharT>
		OutIt output_integer(
				OutIt out, digit_buffer<CharT> const& digits, conversion_options const& options,
				sign_character sign)
		{
			auto out_func = [&]()
			{
				return digits.output(out);
			};

			return output_padded_with_sign<CharT>(out, out_func, digits.count,
					options, sign);
		}


		// Get the sign character required to display the given sign with the given
		// representation of positive numbers
		constexpr sign_character sign_from_format(const bool neg, const pos_sign_type pos)
		{
			if (neg)
			{
				return sign_character::minus;
			}

			switch (pos)
			{
			case pos_sign_type::plus:
				return sign_character::plus;
			case pos_sign_type::space:
				return sign_character::space;
			default:
				return sign_character::none;
			}
		}


		// Format an unsigned integer without validity checks for given flags with
		// given sign and options.
		template<typename CharT, typename OutIt, typename ValueT>
		typename std::enable_if<
				std::is_integral<ValueT>::value && std::is_unsigned<ValueT>::value, OutIt>::type
		format_integer_unchecked(OutIt out, ValueT value, bool negative,
				conversion_options const& options)
		{
			// Special case: Conversion to character requested
			if (options.format == conversion_format::character)
			{
				*out++ = CharT(value);
			}
			else
			{
				digit_buffer<CharT> digits = generate_digits<CharT>(value, options.format);

				out = output_integer(out, digits, options,
						sign_from_format(negative, options.pos_sign));

			}

			return out;
		}


		// Format unsigned integer with checks for flag validity with given sign and options.
		template<typename CharT, typename OutIt, typename ValueT>
		typename std::enable_if<
				std::is_integral<ValueT>::value && std::is_unsigned<ValueT>::value, OutIt>::type
		format_integer(OutIt out, ValueT value, bool negative, conversion_options options)
		{
			if (options.alignment != fill_alignment::intern)
			{
				options.zero_fill = false;
			}

			return format_integer_unchecked<CharT>(out, value, negative, options);
		}


		// Absolute value of given value as the unsigned type with same width as
		// input type (this allows // getting the absolute value of the lowest integer
		// without overflow).
		template<typename ValueT>
		typename std::make_unsigned<ValueT>::type make_positive(ValueT value)
		{
			if (value >= 0)
			{
				return static_cast<typename std::make_unsigned<ValueT>::type>(value);
			}
			else
			{
				return ~(static_cast<typename std::make_unsigned<ValueT>::type>(value) - 1U);
			}
		}

		// String formatter for C-Strings
		template<typename CharT, typename OutIt>
		OutIt format_element(OutIt out, conversion_options const& options, CharT const* value)
		{
			CharT const* end = value;
			while (*end != CharT('\0'))
			{
				++end;
			}

			return format_string<CharT>(out, options, value, end);
		}


		// String formatter for C++ strings
		template<typename CharT, typename OutIt>
		OutIt format_element(OutIt out, conversion_options const& options,
				std::basic_string_view<CharT> value)
		{
			return format_string<CharT>(out, options, value.begin(), value.end());
		}


		// If there are not value left to convert, just copy the rest of the input.
		// Ignore further conversion specifiers.
		template<typename OutIt, typename InputIt>
		OutIt format_it(OutIt out, InputIt start, InputIt const end)
		{
			return std::copy(start, end, out);
		}


		// Formatter function for unsigned integers
		template<typename CharT, typename OutIt, typename ValueT>
		typename std::enable_if<
				std::is_integral<ValueT>::value && std::is_unsigned<ValueT>::value, OutIt>::type
		format_element(OutIt out, conversion_options options, ValueT value)
		{
			return format_integer<CharT>(out, value, false, options);
		}


		// Formatter function for a signed integer. Converts the given number bitwise to
		// an unsigned value if the requested conversion is _not_ decimal. For decimal,
		// it passes the absolute value and sign bit appropriately
		template<typename CharT, typename OutIt, typename ValueT>
		typename std::enable_if<
				std::is_integral<ValueT>::value && std::is_signed<ValueT>::value, OutIt>::type
		format_element(OutIt out, conversion_options options, ValueT value)
		{
			if (options.format != conversion_format::normal and
				options.format != conversion_format::decimal)
			{
				return format_integer<CharT>(out,
						static_cast<typename std::make_unsigned<ValueT>::type>(value), false,
						options);
			}
			else
			{
				return format_integer<CharT>(out, make_positive(value), value < 0, options);
			}
		}


// Formatter function for floating point numbers.

#if FLOSSY_FLOAT_METHOD == FLOSSY_FLOAT_METHOD_SSTREAM
		// This method used C++ string streams to convert float values. That means it
		// is precise and easy to implement. String streams use dynamic memory though,
		// so it may have unpredictable timing and be slower than other alternatives.

		template<typename CharT, typename OutIt, typename ValueT>
		typename std::enable_if<std::is_floating_point<ValueT>::value, OutIt>::type
		format_element(OutIt out, conversion_options options, ValueT value)
		{
			if (options.alignment != fill_alignment::intern || std::isinf(value))
			{
				options.zero_fill = false;
			}

			if (std::isnan(value))
			{
				options.zero_fill = false;
				if (options.pos_sign == pos_sign_type::plus)
				{
					options.pos_sign = pos_sign_type::space;
				}
			}


			// Format as char string, convert to wider character types later (in std::copy).
			// This works with char32_t, while using a basic_ostringstream<char32_t> doesn't.
			// I did not investigate further, why it doesn't work. :)
			std::stringstream buffer;
			buffer.precision(options.precision);
			buffer.flags(
					options.format != conversion_format::scientific_float
					? std::ios::fixed
					: std::ios::scientific
			);
			buffer << std::abs(value);

			auto out_func = [&]()
			{
				return std::copy(std::istreambuf_iterator<char>(buffer.rdbuf()),
						std::istreambuf_iterator<char>(), out);
			};

			// The method std::signbit determines if the given floating point number arg is negative.
			// Return value: true if arg is negative, false otherwise.
			bool const isNegative = std::signbit(value);

			return output_padded_with_sign<CharT>(out, out_func, buffer.tellp(), options,
					sign_from_format(isNegative, options.pos_sign));
		}

#elif FLOSSY_FLOAT_METHOD == FLOSSY_FLOAT_METHOD_FAST
#error "Fast (and imprecise) float conversion not implemented, yet."
#elif FLOSSY_FLOAT_METHOD == FLOSSY_FLOAT_METHOD_GRISU
#error "Grisu float conversion not implemented, yet."
#else
#error "FLOSSY_FLOAT_METHOD undefined."
#endif


		// Generic formatting function using iterators
		//
		// This function is the main work horse of flossy. It does all the format
		// string evaluation and formatting of elements.
		// It recursively calls itself (or the "no values remain" overload above) to
		// piecewise construct the output string.
		//
		// Template parameters:
		//   OutIt        OutputIterator type used to write the resulting characters
		//                to. Must accept writes of the same type that dereferencing an
		//                InputIt (see below) yields.
		//   InputIt      ForwardIterator used to read the format string characters
		//                from.
		//   FirstValueT  Type of the first value to use in conversions.
		//   ValueTs      Types of the remaining values to be converted.
		//
		// Parameters:
		//   out          Output iterator to store the resulting string characters.
		//   start        Iterator to beginning of format string.
		//   end          Iterator to the character one element after the last
		//                character of the format string.
		//   first        The first value to be converted.
		//   elements     Remaining values to be used in later conversions.
		//
		// Return value:
		//   Updated 'out' iterator
		//
		// Usage example:
		//
		//   std::string result;
		//   std::string format_str("The first value passed is {}"
		//                          ", and the second is {}!");
		//   auto it = format_it(
		//     std::back_inserter(result), format_str.begin(), format_str.end(),
		//     42, "foo");
		//
		//   'it' can then be used to append further to the string. Instead of string
		//   and back_inserter, every output iterator works, including
		//   ostream_iterator, which can be very useful.
		//
		template<typename OutIt, typename InputIt, typename FirstValueT, typename... ValueTs>
		OutIt format_it(OutIt out, InputIt start, InputIt const end, FirstValueT const& first,
				ValueTs&& ... elements)
		{
			// Copy everything from start to the beginning of the first "real" (i.e. not '{{') conversion
			// specifier to out, transforming {{ into { appropriately.
			// Read conversion specifier, convert one element and recurse to format the rest.

			for (; start != end; ++start)
			{
				auto c = *start;

				if (c == '{')
				{
					ensure_not_equal(++start, end);

					if (*start != '{')
					{
						auto const options = option_reader<InputIt>(start, end).options;
						format_element<typename std::iterator_traits<InputIt>::value_type>(out,
								options,
								first);
						return format_it(out, start, end, std::forward<ValueTs>(elements)...);
					}

					c = '{';
				}

				*out++ = c;
			}

			return out;
		}
	}

	/**
	 * @page Basic Format String.
	 *
	 * Convenience function wrapper for format_it that allows formatting a
	 * format string and values directly into a string and returning that.
	 *
	 * @example
	 * @code
	 * auto result = format("The first value passed is {}, and the second is {}!"s,
	 * 						42, "foo");
	 * @endcode
	 *
	 * @tparam CharT Character type to generate in the output string.
	 * @tparam ValueTs Types of the values to be formatted.
	 *
	 * @param format_str Format string to be used when formatting the string.
	 * 	It will be passed to format_it directly.
	 * @param elements The elements to be formatted. They are passed to
	 * format_it verbatim.
	 *
	 * @return The formatted string.
	 */
	template<typename CharT, typename... ValueTs>
	std::basic_string<CharT>
	format(std::basic_string_view<CharT> format_str, ValueTs&& ... elements)
	{
		// With this if-constexpr, We ensure that the code block will never be
		// called with an empty argument list.
		if constexpr (sizeof...(elements) > 0)
		{
			std::basic_string<CharT> result;
			internal::format_it(std::back_inserter(result), format_str.begin(),
					format_str.end(), std::forward<ValueTs>(elements)...);
			return result;
		}
		else
		{
			// Convert the std::basic_string_view to std::base_string
			return { format_str.begin(), format_str.end() };
		}
	}

	/**
	 * @page Implicit Conversion in Template Deduction Process.
	 *
	 * The documentation of this method is the same that of: Basic Format
	 * String page.
	 *
	 * Current the standard now allow the implicit conversion in the template
	 * deduction process. Is needed added a overload for manage the
	 * std::basic_string<Char> (aka. std::string, std::u32string, etc ...).
	 *
	 * Implicit conversion is not a part of the template deduction process.
	 * References: https://stackoverflow.com/a/22848951
	 */
	template<typename CharT, typename ... ValueTs>
	std::basic_string<CharT>
	format(const std::basic_string<CharT>& format_str, ValueTs&& ... elements)
	{
		// With this if-constexpr, We ensure that the code block will never be
		// called with an empty argument list.
		if constexpr (sizeof...(elements) > 0)
		{
			std::basic_string<CharT> result;
			internal::format_it(std::back_inserter(result), format_str.begin(), format_str.end(),
					std::forward<ValueTs>(elements)...);
			return result;
		}
		else
		{
			// Return the string without modifications.
			return format_str;
		}
	}


	/**
	 * The documentation of this method is the same that of: Basic Format
	 * String page.
	 *
	 * This method is overload for manage const char*. See the why is needed in
	 * the Implicit Conversion in Template Deduction Process page.
	 */
	template<typename CharT, typename... ValueTs>
	std::basic_string<CharT> format(CharT const* format_str, ValueTs&& ... elements)
	{
		return format(std::basic_string_view<CharT>(format_str),
				std::forward<ValueTs>(elements)...);
	}


	// Convenience function wrapper for format_it that allows formatting a format
	// string and values directly to an ostream.
	//
	// Template parameters:
	//   CharT       Character type to generate in the output string.
	//   Traits      Character traits to be used on the stream.
	//   ValueTs     Types of the values to be formatted.
	//
	// Parameters:
	//   ostream     Stream to write the resulting string to.
	//   format_str  Format string to be used when formatting the string. It will
	//               be passed to format_it directly.
	//   elements    The elements to be formatted. They are passed to format_it
	//               verbatim.
	//
	// Return value:
	//   The ostream that was passed in.
	//
	// Usage example:
	//
	//   format(std::cout, "The first value passed is {}, and the second is {}!"s,
	//          42, "foo");
	//
	template<typename CharT, typename Traits, typename... ValueTs>
	std::basic_ostream<CharT, Traits>& format(
			std::basic_ostream<CharT, Traits>& ostream, std::basic_string_view<CharT> format_str,
			ValueTs&& ... elements)
	{
		// With this if-constexpr, We ensure that the code block will never be
		// called with an empty argument list.
		if constexpr (sizeof ... (elements) > 0)
		{
			internal::format_it(std::ostream_iterator<CharT, CharT>(ostream),
					format_str.begin(), format_str.end(),
					std::forward<ValueTs>(elements)...);
			return ostream;
		}
		else
		{
			return (ostream << format_str);
		}
	}


	// Convenience function wrapper for format_it that allows formatting a format
	// string and values directly to an ostream. (C string variant)
	//
	// Template parameters:
	//   CharT       Character type to generate in the output string.
	//   Traits      Character traits to be used on the stream.
	//   ValueTs     Types of the values to be formatted.
	//
	// Parameters:
	//   ostream     Stream to write the resulting string to.
	//   format_str  Format string to be used when formatting the string. It will
	//               be passed to format_it directly.
	//   elements    The elements to be formatted. They are passed to format_it
	//               verbatim.
	//
	// Return value:
	//   The ostream that was passed in.
	//
	// Usage example:
	//
	//   format(std::cout, "The first value passed is {}, and the second is {}!"s,
	//          42, "foo");
	//
	template<typename CharT, typename Traits, typename... ValueTs>
	std::basic_ostream<CharT, Traits>& format(
			std::basic_ostream<CharT, Traits>& ostream, CharT const* format_str,
			ValueTs&& ... elements)
	{
		format(ostream, std::basic_string_view<CharT>(format_str),
				std::forward<ValueTs>(elements)...);
		return ostream;
	}


}

#endif
