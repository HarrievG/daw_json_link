// The MIT License (MIT)
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

#include <cstdint>
#include <daw/json/daw_json_link.h>
#include <optional>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

struct events_value_t {
	int64_t id;
	std::optional<std::string_view> logo;
	std::string_view name;
	std::vector<int64_t> subTopicIds;
	std::vector<int64_t> topicIds;
}; // events_value_t

auto describe_json_class( events_value_t ) {
	using namespace daw::json;
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const name[] = "name";
	static constexpr char const subTopicIds[] = "subTopicIds";
	static constexpr char const topicIds[] = "topicIds";
	return daw::json::class_description_t<
	  json_number<id, int64_t>,
	  json_nullable<json_string<logo, std::string_view>>,
	  json_string<name, std::string_view>,
	  json_array<subTopicIds, std::vector<int64_t>,
	             json_number<no_name, int64_t>>,
	  json_array<topicIds, std::vector<int64_t>,
	             json_number<no_name, int64_t>>>{};
}

auto to_json_data( events_value_t const &value ) {
	return std::forward_as_tuple( value.id, value.logo, value.name,
	                              value.subTopicIds, value.topicIds );
}

struct prices_element_t {
	int64_t amount;
	int64_t audienceSubCategoryId;
	int64_t seatCategoryId;
}; // prices_element_t

auto describe_json_class( prices_element_t ) {
	using namespace daw::json;
	static constexpr char const amount[] = "amount";
	static constexpr char const audienceSubCategoryId[] = "audienceSubCategoryId";
	static constexpr char const seatCategoryId[] = "seatCategoryId";
	return daw::json::class_description_t<
	  json_number<amount, int64_t>, json_number<audienceSubCategoryId, int64_t>,
	  json_number<seatCategoryId, int64_t>>{};
}

auto to_json_data( prices_element_t const &value ) {
	return std::forward_as_tuple( value.amount, value.audienceSubCategoryId,
	                              value.seatCategoryId );
}

struct areas_element_t {
	int64_t areaId;
}; // areas_element_t

auto describe_json_class( areas_element_t ) {
	using namespace daw::json;
	static constexpr char const areaId[] = "areaId";
	return daw::json::class_description_t<json_number<areaId, int64_t>>{};
}

auto to_json_data( areas_element_t const &value ) {
	return std::forward_as_tuple( value.areaId );
}

struct seatCategories_element_t {
	std::vector<areas_element_t> areas;
	int64_t seatCategoryId;
}; // seatCategories_element_t

auto describe_json_class( seatCategories_element_t ) {
	using namespace daw::json;
	static constexpr char const areas[] = "areas";
	static constexpr char const seatCategoryId[] = "seatCategoryId";
	return daw::json::class_description_t<
	  json_array<areas, std::vector<areas_element_t>,
	             json_class<no_name, areas_element_t>>,
	  json_number<seatCategoryId, int64_t>>{};
}

auto to_json_data( seatCategories_element_t const &value ) {
	return std::forward_as_tuple( value.areas, value.seatCategoryId );
}

struct performances_element_t {
	int64_t eventId;
	int64_t id;
	std::optional<std::string_view> logo;
	std::vector<prices_element_t> prices;
	std::vector<seatCategories_element_t> seatCategories;
	int64_t start;
	std::string_view venueCode;
}; // performances_element_t

auto describe_json_class( performances_element_t ) {
	using namespace daw::json;
	static constexpr char const eventId[] = "eventId";
	static constexpr char const id[] = "id";
	static constexpr char const logo[] = "logo";
	static constexpr char const prices[] = "prices";
	static constexpr char const seatCategories[] = "seatCategories";
	static constexpr char const start[] = "start";
	static constexpr char const venueCode[] = "venueCode";
	return daw::json::class_description_t<
	  json_number<eventId, int64_t>, json_number<id, int64_t>,
	  json_nullable<json_string<logo, std::string_view>>,
	  json_array<prices, std::vector<prices_element_t>,
	             json_class<no_name, prices_element_t>>,
	  json_array<seatCategories, std::vector<seatCategories_element_t>,
	             json_class<no_name, seatCategories_element_t>>,
	  json_number<start, int64_t>, json_string<venueCode, std::string_view>>{};
}

auto to_json_data( performances_element_t const &value ) {
	return std::forward_as_tuple( value.eventId, value.id, value.logo,
	                              value.prices, value.seatCategories, value.start,
	                              value.venueCode );
}

struct venueNames_t {
	std::string_view pleyel_pleyel;
}; // venueNames_t

auto describe_json_class( venueNames_t ) {
	using namespace daw::json;
	static constexpr char const PLEYEL_PLEYEL[] = "PLEYEL_PLEYEL";
	return daw::json::class_description_t<
	  json_string<PLEYEL_PLEYEL, std::string_view>>{};
}

auto to_json_data( venueNames_t const &value ) {
	return std::forward_as_tuple( value.pleyel_pleyel );
}

struct citm_object_t {
	std::unordered_map<std::string_view, std::string_view> areaNames;
	std::unordered_map<std::string_view, std::string_view>
	  audienceSubCategoryNames;
	std::unordered_map<std::string_view, events_value_t> events;
	std::vector<performances_element_t> performances;
	std::unordered_map<std::string_view, std::string_view> seatCategoryNames;
	std::unordered_map<std::string_view, std::string_view> subTopicNames;
	std::unordered_map<std::string_view, std::string_view> topicNames;
	std::unordered_map<std::string_view, std::vector<int64_t>> topicSubTopics;
	std::optional<venueNames_t> venueNames;
}; // citm_object_t

auto describe_json_class( citm_object_t ) {
	using namespace daw::json;
	static constexpr char const areaNames[] = "areaNames";
	static constexpr char const audienceSubCategoryNames[] =
	  "audienceSubCategoryNames";
	static constexpr char const events[] = "events";
	static constexpr char const performances[] = "performances";
	static constexpr char const seatCategoryNames[] = "seatCategoryNames";
	static constexpr char const subTopicNames[] = "subTopicNames";
	static constexpr char const topicNames[] = "topicNames";
	static constexpr char const topicSubTopics[] = "topicSubTopics";
	static constexpr char const venueNames[] = "venueNames";
	return daw::json::class_description_t<
	  json_key_value<areaNames,
	                 std::unordered_map<std::string_view, std::string_view>,
	                 json_string<no_name, std::string_view>>,
	  json_key_value<audienceSubCategoryNames,
	                 std::unordered_map<std::string_view, std::string_view>,
	                 json_string<no_name, std::string_view>>,
	  json_key_value<events, std::unordered_map<std::string_view, events_value_t>,
	                 json_class<no_name, events_value_t>>,
	  json_array<performances, std::vector<performances_element_t>,
	             json_class<no_name, performances_element_t>>,
	  json_key_value<seatCategoryNames,
	                 std::unordered_map<std::string_view, std::string_view>,
	                 json_string<no_name, std::string_view>>,
	  json_key_value<subTopicNames,
	                 std::unordered_map<std::string_view, std::string_view>,
	                 json_string<no_name, std::string_view>>,
	  json_key_value<topicNames,
	                 std::unordered_map<std::string_view, std::string_view>,
	                 json_string<no_name, std::string_view>>,
	  json_key_value<
	    topicSubTopics,
	    std::unordered_map<std::string_view, std::vector<int64_t>>,
	    json_array<no_name, std::vector<int64_t>, json_number<no_name, int64_t>>>,
	  json_nullable<json_class<venueNames, venueNames_t>>>{};
}

auto to_json_data( citm_object_t const &value ) {
	return std::forward_as_tuple(
	  value.areaNames, value.audienceSubCategoryNames, value.events,
	  value.performances, value.seatCategoryNames, value.subTopicNames,
	  value.topicNames, value.topicSubTopics, value.venueNames );
}
