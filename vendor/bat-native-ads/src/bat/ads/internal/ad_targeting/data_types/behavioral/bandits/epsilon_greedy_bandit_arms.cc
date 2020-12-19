/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/ad_targeting/data_types/behavioral/bandits/epsilon_greedy_bandit_arms.h"

#include <utility>

#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/strings/string_number_conversions.h"
#include "bat/ads/internal/logging.h"

namespace ads {
namespace ad_targeting {

namespace {

const char kArmSegmentKey[] = "segment";
const char kArmValueKey[] = "value";
const char kArmPullsKey[] = "pulls";

bool GetArmFromDictionary(
    const base::DictionaryValue* dictionary,
    EpsilonGreedyBanditArmInfo* info) {
  DCHECK(dictionary);
  if (!dictionary) {
    return false;
  }

  DCHECK(info);
  if (!info) {
    return false;
  }

  EpsilonGreedyBanditArmInfo arm;

  // Segment
  const auto* segment = dictionary->FindStringKey(kArmSegmentKey);
  if (!segment) {
    return false;
  }
  arm.segment = *segment;

  // Pulls
  const auto* pulls = dictionary->FindStringKey(kArmPullsKey);
  if (!pulls) {
    return false;
  }
  if (!base::StringToUint64(*pulls, &arm.pulls)) {
    return false;
  }

  // Value
  const auto* value =
      dictionary->FindStringKey(kArmValueKey);
  if (!value) {
    return false;
  }

  if (!base::StringToDouble(*value, &arm.value)) {
    return false;
  }

  *info = arm;
  return true;
}

EpsilonGreedyBanditArmMap GetArmsFromDictionary(
    const base::DictionaryValue* dictionary) {
  EpsilonGreedyBanditArmMap arms;

  DCHECK(dictionary);
  if (!dictionary) {
    return arms;
  }

  for (const auto& value : dictionary->DictItems()) {
    if (!value.second.is_dict()) {
      NOTREACHED();
      continue;
    }

    const base::DictionaryValue* arm_dictionary = nullptr;
    value.second.GetAsDictionary(&arm_dictionary);
    if (!arm_dictionary) {
      NOTREACHED();
      continue;
    }

    EpsilonGreedyBanditArmInfo arm;
    if (!GetArmFromDictionary(arm_dictionary, &arm)) {
      NOTREACHED();
      continue;
    }

    arms[value.first] = arm;
  }

  return arms;
}

}  // namespace

EpsilonGreedyBanditArms::EpsilonGreedyBanditArms() = default;

EpsilonGreedyBanditArms::~EpsilonGreedyBanditArms() = default;

EpsilonGreedyBanditArmMap EpsilonGreedyBanditArms::FromJson(
    const std::string& json) {
  EpsilonGreedyBanditArmMap arms;
  base::Optional<base::Value> value = base::JSONReader::Read(json);
  if (!value || !value->is_dict()) {
    return arms;
  }

  base::DictionaryValue* dictionary = nullptr;
  if (!value->GetAsDictionary(&dictionary)) {
    return arms;
  }

  arms = GetArmsFromDictionary(dictionary);
  return arms;
}

std::string EpsilonGreedyBanditArms::ToJson(
    const EpsilonGreedyBanditArmMap& arms) {
  base::Value dictionary(base::Value::Type::DICTIONARY);

  for (const auto& arm : arms) {
    base::Value dictionary2(base::Value::Type::DICTIONARY);
    dictionary2.SetKey(kArmSegmentKey, base::Value(arm.first));
    dictionary2.SetKey(kArmPullsKey,
        base::Value(std::to_string(arm.second.pulls)));
    dictionary2.SetKey(kArmValueKey,
        base::Value(std::to_string(arm.second.value)));
    dictionary.SetKey(arm.first, std::move(dictionary2));
  }

  std::string json;
  base::JSONWriter::Write(dictionary, &json);

  return json;
}

///////////////////////////////////////////////////////////////////////////////

}  // namespace ad_targeting
}  // namespace ads
