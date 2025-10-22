#ifndef FEATURES_H
#define FEATURES_H

void extract_features(const float *data, uint length, float *features) {
  // Sum, Absolute Change Sum, Min, Max
  float sum = 0;
  float sum_abs_change = 0;
  float min = data[0];
  float max = data[0];
  for (int i = 0; i < length; i++) {
    sum += data[i];
    if (i > 0) {
      sum_abs_change += abs(data[i] - data[i - 1]);
    }
    if (data[i] < min) {
      min = data[i];
    }
    if (data[i] > max) {
      max = data[i];
    }
  }

  // Mean, Absolute Change Mean
  float mean = sum / length;
  float mean_abs_change = sum_abs_change / (length - 1);

  // Energy, Variance, 3rd/4th Central Moment, Below/Above Mean Count
  float abs_energy = 0;
  float variance = 0;
  float central_moment_3 = 0;
  float central_moment_4 = 0;
  float count_below_mean = 0;
  float count_above_mean = 0;
  for (int i = 0; i < length; i++) {
    abs_energy += pow(data[i], 2);
    variance += pow(data[i] - mean, 2);
    central_moment_3 += pow(data[i] - mean, 3);
    central_moment_4 += pow(data[i] - mean, 4);
    if (data[i] < mean) {
      count_below_mean++;
    }
    if (data[i] > mean) {
      count_above_mean++;
    }
  }
  variance /= length;
  central_moment_3 /= length;
  central_moment_4 /= length;
  
  // Standard Deviation
  float std_dev = sqrt(variance);
  
  // Autocorrelation
  float autocorr[9] = {0};
  for (int lag = 1; lag <= 9; lag++) {
    for (int i = 0; i < length - lag; i++) {
      autocorr[lag - 1] += (data[i] - mean) * (data[i + lag] - mean);
    }
    autocorr[lag - 1] /= (length - lag) * variance;
  }

  // Skewness, Kurtosis
  float skewness = variance == 0.0 ? 0.0 : central_moment_3 / pow(variance, 1.5);
  float kurtosis = variance == 0.0 ? 0.0 : central_moment_4 / pow(variance, 2) - 3.0;

  // Store features in the output array
  uint idx = 0;
  features[idx++] = abs_energy;
  for (int i = 0; i < 9; i++) {
    features[idx++] = autocorr[i];
  }
  features[idx++] = count_above_mean;
  features[idx++] = count_below_mean;
  features[idx++] = kurtosis;
  features[idx++] = max;
  features[idx++] = mean;
  features[idx++] = mean_abs_change;
  features[idx++] = min;
  features[idx++] = skewness;
  features[idx++] = std_dev;
  features[idx++] = variance;
}

#endif // FEATURES_H