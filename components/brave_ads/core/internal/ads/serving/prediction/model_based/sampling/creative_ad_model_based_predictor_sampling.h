/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_ADS_CORE_INTERNAL_ADS_SERVING_PREDICTION_MODEL_BASED_SAMPLING_CREATIVE_AD_MODEL_BASED_PREDICTOR_SAMPLING_H_
#define BRAVE_COMPONENTS_BRAVE_ADS_CORE_INTERNAL_ADS_SERVING_PREDICTION_MODEL_BASED_SAMPLING_CREATIVE_AD_MODEL_BASED_PREDICTOR_SAMPLING_H_

#include <limits>
#include <ostream>

#include "base/notreached.h"
#include "base/numerics/ranges.h"
#include "base/rand_util.h"
#include "brave/components/brave_ads/core/internal/ads/serving/prediction/model_based/creative_ad_model_based_predictor_info.h"
#include "brave/components/brave_ads/core/internal/ads/serving/prediction/model_based/sampling/creative_ad_model_based_predictor_sampling_util.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace brave_ads {

template <typename T>
absl::optional<T> MaybeSampleCreativeAd(
    const CreativeAdPredictorList<T>& creative_ad_predictors) {
  if (creative_ad_predictors.empty()) {
    return absl::nullopt;
  }

  const double normalizing_constant =
      CalculateNormalizingConstantForCreativeAdPredictors(
          creative_ad_predictors);
  if (normalizing_constant <= 0.0 ||
      base::IsApproximatelyEqual(normalizing_constant, 0.0,
                                 std::numeric_limits<double>::epsilon())) {
    return absl::nullopt;
  }

  const double rand = base::RandDouble();

  double sum = 0.0;

  for (const auto& creative_ad_predictor : creative_ad_predictors) {
    const double probability =
        creative_ad_predictor.score / normalizing_constant;

    sum += probability;

    if (rand <= sum || base::IsApproximatelyEqual(
                           rand, sum, std::numeric_limits<double>::epsilon())) {
      return creative_ad_predictor.creative_ad;
    }
  }

  NOTREACHED_NORETURN() << "Sum should always be less than probability";
}

}  // namespace brave_ads

#endif  // BRAVE_COMPONENTS_BRAVE_ADS_CORE_INTERNAL_ADS_SERVING_PREDICTION_MODEL_BASED_SAMPLING_CREATIVE_AD_MODEL_BASED_PREDICTOR_SAMPLING_H_
