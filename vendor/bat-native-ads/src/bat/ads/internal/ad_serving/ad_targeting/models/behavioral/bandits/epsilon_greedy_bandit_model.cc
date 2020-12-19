/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/ad_serving/ad_targeting/models/behavioral/bandits/epsilon_greedy_bandit_model.h"

#include <stdint.h>

#include <algorithm>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "base/rand_util.h"
#include "bat/ads/internal/ad_targeting/data_types/behavioral/bandits/epsilon_greedy_bandit_aliases.h"
#include "bat/ads/internal/ads_client_helper.h"
#include "bat/ads/internal/features/bandits/epsilon_greedy_bandit_features.h"
#include "bat/ads/internal/logging.h"
#include "bat/ads/pref_names.h"

namespace ads {
namespace ad_targeting {
namespace model {

namespace {

const size_t kTopSegmentCount = 3;
// const int kIntMax = std::numeric_limits<int>::max();

SegmentList ToSegmentList(
    const ArmList arms) {
  SegmentList segments;

  for (const auto& arm : arms) {
    segments.push_back(arm.segment);
  }

  return segments;
}

ArmList ToArmList(
    const EpsilonGreedyBanditArmMap& arms) {
  ArmList arm_list;

  for (const auto& arm : arms) {
    arm_list.push_back(arm.second);
  }

  return arm_list;
}

SegmentList GetTopArms(
    const ArmList& arms,
    const int count) {
  ArmList top_arms(count);
  std::partial_sort_copy(arms.begin(), arms.end(), top_arms.begin(),
      top_arms.end(), [](const EpsilonGreedyBanditArmInfo& lhs,
          const EpsilonGreedyBanditArmInfo& rhs) {
    // TOOD(Moritz Haller): Try and send error to Terry
    // Break ties randomly >> write own sort
    // if (lhs.value == rhs.value) {
    //   return base::RandInt(0, kIntMax) % 2 == 1;
    // }
    return lhs.value > rhs.value;
  });

  return ToSegmentList(top_arms);
}

}  // namespace

EpsilonGreedyBandit::EpsilonGreedyBandit() = default;

EpsilonGreedyBandit::~EpsilonGreedyBandit() = default;

SegmentList EpsilonGreedyBandit::GetSegments() const {
  const std::string json = AdsClientHelper::Get()->GetStringPref(
      prefs::kEpsilonGreedyBanditArms);
  const EpsilonGreedyBanditArmMap arms =
      EpsilonGreedyBanditArms::FromJson(json);
  return ChooseAction(arms);
}

///////////////////////////////////////////////////////////////////////////////

SegmentList EpsilonGreedyBandit::ChooseAction(
    const EpsilonGreedyBanditArmMap& arms) const {
  SegmentList segments;
  if (arms.size() < kTopSegmentCount) {
    return segments;
  }

  // Explore
  const double epsilon = features::GetEpsilonGreedyBanditEpsilonValue();
  if (base::RandDouble() < epsilon) {
    for (const auto& arm : arms) {
      segments.push_back(arm.first);
    }

    // Sample without replacement
    base::RandomShuffle(begin(segments), end(segments));
    segments.resize(kTopSegmentCount);
    return segments;
  }

  // Exploit
  ArmList arm_list = ToArmList(arms);
  segments = GetTopArms(arm_list, kTopSegmentCount);
  return segments;
}

}  // namespace model
}  // namespace ad_targeting
}  // namespace ads
