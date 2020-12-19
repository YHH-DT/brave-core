/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_AD_TARGETING_DATA_TYPES_BEHAVIORAL_BANDITS_EPSILON_GREEDY_BANDIT_ARM_INFO_H_
#define BAT_ADS_INTERNAL_AD_TARGETING_DATA_TYPES_BEHAVIORAL_BANDITS_EPSILON_GREEDY_BANDIT_ARM_INFO_H_

#include <stdint.h>

#include <string>
#include <vector>

#include "base/values.h"

namespace ads {

struct EpsilonGreedyBanditArmInfo {
  EpsilonGreedyBanditArmInfo();

  EpsilonGreedyBanditArmInfo(
    const std::string& segment,
    const double value,
    const uint64_t pulls);

  ~EpsilonGreedyBanditArmInfo();

  bool operator==(
      const EpsilonGreedyBanditArmInfo& rhs) const;
  bool operator!=(
      const EpsilonGreedyBanditArmInfo& rhs) const;

  bool IsValid() const;

  std::string segment;
  double value = 0.0;
  uint64_t pulls = 0;
};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_AD_TARGETING_DATA_TYPES_BEHAVIORAL_BANDITS_EPSILON_GREEDY_BANDIT_ARM_INFO_H_