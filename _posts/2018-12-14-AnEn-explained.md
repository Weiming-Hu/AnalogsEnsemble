---
layout: post
title: The Analog Ensemble Technique Explained
tags:
  - document
---

## Schematic Diagram

The following schematic diagram shows the four steps to generate a four-member ensemble forecast.

![AnEn-scheme](https://github.com/Weiming-Hu/AnalogsEnsemble/raw/gh-pages/assets/posts/2018-12-14-AnEn-explained/AnEn-schema.png)

- Step 1: The process starts with a current deterministic multivariate prediction and a set of historical predictions from a deterministic weather model. The multivariate prediction includes surface temperature, humidity, wind speed, and so on. Corresponding observations to each historical forecasts are also collected.
- Step 2: A number of historical predictions are identified based on their similarity to the current multivariate prediction. This similarity is also time-dependent, meaning that, instead of point-to-point comparison, it also compares the trend of each weather variable within a short time range.
- Step 3: The corresponding observations associated with the identified historical predictions are selected.
- Step 4: These **observations** become ensemble members in the final forecast.

## Simplified Example for Temperature Forecasts

Please navigate through the following slides to see the example.

<iframe src="https://onedrive.live.com/embed?cid=BCFC2A6DB1591BCA&amp;resid=BCFC2A6DB1591BCA%212248&amp;authkey=AHAAoXCKyl1NiTs&amp;em=2&amp;wdAr=1.7777777777777777&amp;wdEaa=1" width="1186px" height="691px" frameborder="0">This is an embedded <a target="_blank" href="https://office.com">Microsoft Office</a> presentation, powered by <a target="_blank" href="https://office.com/webapps">Office Online</a>.</iframe>

*Animation credited to [Laura Clemente-Harding](http://geoinf.psu.edu/people.shtml) and [Guido Cervone](http://geoinf.psu.edu/people.shtml)*

- Step 1: A deterministic model has been running for a week and a new prediction is generated from the model. Red dots are temperature observations, and black dots are model predictions.
- Step 2: By comparing current and historical model predictions (black dots), most similar past forecasts are identified.
- Step 3: The corresponding observations are selected that are associated with the identified past predictions.
- Step 4: These **observations** become ensemble members in the final forecast.

Of course, in reality, the similarity metric is a time-dependent and multivariate metric.

## References

- [Analog Ensemble Package](https://weiming-hu.github.io/AnalogsEnsemble/)
- [A Beginners Introduction to the Analog Ensemble Technique](https://ral.ucar.edu/sites/default/files/public/images/events/WISE_documentation_20170725_Final.pdf)
- [Predictor-weighting strategies for probabilistic wind power forecasting with an analog ensemble](https://www.schweizerbart.de/papers/metz/detail/24/84737/Predictor_weighting_strategies_for_probabilistic_wind_power_forecasting_with_an_analog_ensemble)
- [Short-term photovoltaic power forecasting using Artificial Neural Networks and an Analog Ensemble](https://www.sciencedirect.com/science/article/pii/S0960148117301386)
- [Probabilistic Weather Prediction with an Analog Ensemble](https://journals.ametsoc.org/doi/10.1175/MWR-D-12-00281.1)
