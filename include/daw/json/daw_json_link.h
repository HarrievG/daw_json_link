﻿// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>
#include <tuple>

#include <daw/daw_algorithm.h>
#include <daw/daw_array.h>
#include <daw/daw_bounded_string.h>
#include <daw/daw_cxmath.h>
#include <daw/daw_exception.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>
#include <daw/iso8601/daw_date_formatting.h>
#include <daw/iso8601/daw_date_parsing.h>
#include <daw/iterator/daw_back_inserter.h>

#include "impl/daw_json_link_impl.h"

namespace daw::json {
	template<typename... JsonMembers>
	struct class_description_t {
		template<typename OutputIterator, typename... Args>
		static constexpr OutputIterator serialize( OutputIterator it,
		                                           std::tuple<Args...> &&args ) {
			static_assert( sizeof...( Args ) == sizeof...( JsonMembers ),
			               "Argument count is incorrect" );

			static_assert( ( impl::is_a_json_type_v<JsonMembers> and ... ),
			               "Only value json types can be used" );
			return impl::serialize_json_class<JsonMembers...>(
			  it, std::index_sequence_for<Args...>{}, std::move( args ) );
		}

		template<typename Result>
		static constexpr decltype( auto ) parse( std::string_view sv ) {
			return impl::parse_json_class<Result, JsonMembers...>(
			  sv, std::index_sequence_for<JsonMembers...>{} );
		}

		template<typename Result, typename First, typename Last>
		static constexpr decltype( auto )
		parse( impl::IteratorRange<First, Last> &rng ) {
			return impl::parse_json_class<Result, JsonMembers...>(
			  rng, std::index_sequence_for<JsonMembers...>{} );
		}
	};

	// Member types
	template<typename JsonMember>
	struct json_nullable {
		using i_am_a_json_type = typename JsonMember::i_am_a_json_type;
		static constexpr auto name = JsonMember::name;
		static constexpr auto expected_type = JsonParseTypes::Null;
		using parse_to_t = typename JsonMember::parse_to_t;
		using constructor_t = typename JsonMember::constructor_t;
		using sub_type = JsonMember;
		static_assert( std::is_invocable_v<constructor_t>,
		               "Specified constructor must be callable without arguments" );
	};

	template<JSONNAMETYPE Name, typename T = double,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::never,
	         typename Constructor = daw::construct_a_t<T>, bool RangeCheck = false>
	struct json_number {
		static_assert( std::is_invocable_v<Constructor, T>,
		               "Constructor must be callable with T" );

		using i_am_a_json_type = void;
		static constexpr auto name = Name;
		static constexpr auto expected_type = JsonParseTypes::Number;
		static constexpr auto literal_as_string = LiteralAsString;
		static constexpr auto range_check = RangeCheck;
		using parse_to_t = T;
		using constructor_t = Constructor;
	};

	template<JSONNAMETYPE Name, typename T = double,
	         LiteralAsStringOpt LiteralAsString = LiteralAsStringOpt::never,
	         typename Constructor = daw::construct_a_t<T>>
	using json_checked_number =
	  json_number<Name, T, LiteralAsString, Constructor, true>;

	template<JSONNAMETYPE Name, typename T = bool,
	         typename Constructor = daw::construct_a_t<T>>
	struct json_bool {
		static_assert( std::is_constructible_v<T, bool>,
		               "Supplied type but be constructable from a bool" );
		static_assert( std::is_invocable_v<Constructor, T>,
		               "Constructor must be callable with T" );

		static_assert( std::is_convertible_v<bool, T>,
		               "Supplied result type must be convertable from bool" );
		using i_am_a_json_type = void;
		static constexpr auto name = Name;
		static constexpr auto expected_type = JsonParseTypes::Bool;
		using parse_to_t = T;
		using constructor_t = Constructor;
	};

	template<JSONNAMETYPE Name, typename T = std::string,
	         typename Constructor = daw::construct_a_t<T>,
	         bool EmptyStringNull = false>
	struct json_string {
		static_assert(
		  std::is_invocable_v<Constructor, char const *, size_t>,
		  "Constructor must be callable with a char const * and a size_t" );

		using i_am_a_json_type = void;
		static constexpr auto name = Name;
		static constexpr auto expected_type = JsonParseTypes::String;
		using parse_to_t = T;
		using constructor_t = Constructor;
		static constexpr bool empty_is_null = EmptyStringNull;
	};

	/// Link to a JSON string representing a date
	/// \tparam Name name of JSON member to link to
	/// \tparam T C++ type to consruct, by default is a time_point
	/// \tparam Constructor A Callable used to construct a T.
	/// Must accept a char pointer and size as argument to the date/time string.
	template<JSONNAMETYPE Name,
	         typename T = std::chrono::time_point<std::chrono::system_clock,
	                                              std::chrono::milliseconds>,
	         typename Constructor = parse_js_date>
	struct json_date {
		static_assert(
		  std::is_invocable_v<Constructor, char const *, size_t>,
		  "Constructor must be callable with a char const * and a size_t" );

		using i_am_a_json_type = void;
		static constexpr auto name = Name;
		static constexpr auto expected_type = JsonParseTypes::Date;
		using parse_to_t = T;
		using constructor_t = Constructor;
	};

	///
	/// \tparam Name name of JSON member to link to
	/// \tparam T C++ type being parsed to.  Must have a describe_json_class
	/// overload
	/// \tparam Constructor A callable used to construct T.  The
	/// default supports normal and aggregate construction
	template<JSONNAMETYPE Name, typename T,
	         typename Constructor = daw::construct_a_t<T>>
	struct json_class {
		using i_am_a_json_type = void;
		static constexpr auto name = Name;
		static constexpr auto expected_type = JsonParseTypes::Class;
		using parse_to_t = T;
		using constructor_t = Constructor;
	};

	template<JSONNAMETYPE Name, typename T,
	         typename FromConverter = custom_from_converter_t<T>,
	         typename ToConverter = custom_to_converter_t<T>,
	         bool IsString = true>
	struct json_custom {
		using i_am_a_json_type = void;
		static constexpr auto const name = Name;
		static constexpr auto expected_type = JsonParseTypes::Custom;
		using parse_to_t = T;
		using to_converter_t = ToConverter;
		using from_converter_t = FromConverter;
		static constexpr auto const is_string = IsString;
	};

	/// Link to a JSON array
	/// \tparam Name name of JSON member to link to
	/// \tparam Container type of C++ container being constructed(e.g.
	/// vector<int>)
	/// \tparam JsonElement Json type being parsed e.g. json_number,
	/// json_string...
	/// \tparam Constructor A callable used to make Container,
	/// default will use the Containers constructor.  Both normal and aggregate
	/// are supported \tparam Appender Callable used to add items to the
	/// container.  The parsed type from JsonElement
	///	passed to it
	template<JSONNAMETYPE Name, typename Container, typename JsonElement,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_appender<Container>>
	struct json_array {
		using i_am_a_json_type = void;
		static constexpr auto name = Name;
		static constexpr auto expected_type = JsonParseTypes::Array;
		using parse_to_t = Container;
		using constructor_t = Constructor;
		using appender_t = Appender;
		using json_element_t = JsonElement;

		static_assert( impl::is_a_json_type_v<JsonElement> );
	};

	/// Map a KV type json class { "Key String": ValueType, ... }
	/// to a c++ class.  Keys are always string like and the destination
	/// needs to be constructable with a pointer, size
	/// \tparam Name name of JSON member to link to
	/// \tparam Container type to put values in
	/// \tparam JsonValueType Json type of value in kv pair( e.g. json_number,
	/// json_string, ... ) \tparam JsonKeyType type of key in kv pair \tparam
	/// Constructor A callable used to make Container, default will use the
	/// Containers constructor.  Both normal and aggregate are supported \tparam
	/// Appender A callable used to add elements to container.
	template<JSONNAMETYPE Name, typename Container, typename JsonValueType,
	         typename JsonKeyType = json_string<no_name>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_kv_appender<Container>>
	struct json_key_value {
		using i_am_a_json_type = void;
		static constexpr auto name = Name;
		static constexpr auto expected_type = JsonParseTypes::KeyValue;
		using parse_to_t = Container;
		using constructor_t = Constructor;
		using appender_t = Appender;
		using json_element_t = JsonValueType;
		using json_key_t = JsonKeyType;

		static_assert( impl::is_a_json_type_v<JsonValueType> );
		static_assert( impl::is_a_json_type_v<JsonKeyType> );
	};

	template<typename T>
	constexpr T from_json( std::string_view json_data ) {
		static_assert( impl::has_json_parser_description_v<T>,
		               "A function call describe_json_class must exist for type." );

		using desc_t = impl::json_parser_description_t<T>;

		return desc_t::template parse<T>( json_data );
	}

	template<typename T, typename First, typename Last>
	constexpr T from_json( impl::IteratorRange<First, Last> &rng ) {
		static_assert( impl::has_json_parser_description_v<T>,
		               "A function call describe_json_class must exist for type." );

		auto result = impl::json_parser_description_t<T>::template parse<T>( rng );
		rng.trim_left( );
		return result;
	}

	template<typename Result = std::string, typename T>
	constexpr Result to_json( T &&value ) {
		static_assert(
		  impl::has_json_parser_description_v<T>,
		  "A function called describe_json_class must exist for type." );
		static_assert( impl::has_json_to_json_data_v<T>,
		               "A function called to_json_data must exist for type." );

		Result result{};
		impl::json_parser_description_t<T>::template serialize(
		  daw::back_inserter( result ), to_json_data( std::forward<T>( value ) ) );
		return result;
	}

	template<typename JsonElement,
	         typename Container = std::vector<typename JsonElement::parse_to_t>,
	         typename Constructor = daw::construct_a_t<Container>,
	         typename Appender = impl::basic_appender<Container>>
	constexpr auto from_json_array( std::string_view json_data ) {
		using parser_t =
		  json_array<no_name, Container, JsonElement, Constructor, Appender>;

		using impl::data_size::data;
		using impl::data_size::size;

		auto rng = impl::IteratorRange( json_data.begin( ), json_data.end( ) );

		rng.trim_left( );

		return impl::parse_value<parser_t>( ParseTag<JsonParseTypes::Array>{},
		                                    rng );
	}

	template<typename Result = std::string, typename Container>
	constexpr Result to_json_array( Container &&c ) {
		static_assert(
		  daw::traits::is_container_like_v<daw::remove_cvref_t<Container>>,
		  "Supplied container must support begin( )/end( )" );

		Result result = "[";
		for( auto const &v : c ) {
			result += to_json( v );
			result += ',';
		}
		result.back( ) = ']';
		return result;
	}
} // namespace daw::json
