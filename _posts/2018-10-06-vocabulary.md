# Vocabulary for Parallel Ensemble Forecast Package

<!-- vim-markdown-toc GitLab -->

* [Vocabulary Lists](#vocabulary-lists)
    * [Analog Ensemble (AnEn)](#analog-ensemble-anen)
    * [Forecast Lead Time (FLT)](#forecast-lead-time-flt)
    * [Similarity Matrix](#similarity-matrix)
    * [Mapping Matrix](#mapping-matrix)
* [References](#references)

<!-- vim-markdown-toc -->

## Vocabulary List

### Analog Ensemble (AnEn)

AnEn is a technique to generate probabilistic forecasts from deterministic predictions and the corresponding historical observations. It is an efficient method to produce uncertainty information without running weather prediction models mutliple times.

In the `RAnEn` library, `AnEn` is also the class name of the returned value of the function `generateAnalogs`. You can use `print` function to analyze the members of the class.

### Forecast Lead Time (FLT)

In meteorology, FLT means the length of time between the issuance of a forecast and the occurrence of the phenomena that were predicted. For example, a forecast for surface temperature at the beginning of the day January 1st, 2018, with a lead time of 6 hours will be the predicted surface temperature at 6 AM. Usually this interval is represented using the number of seconds from the initial timestamp, therefore 6 hours become 6 * 60 * 60 = 21600 seconds.

### Similarity Matrix

This is a multi-dimensional matrix with similarity values between test and search forecasts. Each test forecast at a given station/grid point, a given timestamp, and a given FLT, is compared with search forecasts for all search stations/grid points and all search times. Therefore, there are in total `# of test forecast stations/grid points X # of test forecast times X # of test forecast FLTs X # of search forecast stations X # of search forecast times` similairity values.

### Mapping Matrix

The Mapping matrix is a two-dimensional matrix that records the matching between forecast times/FLTs and observation times. Since forecasts have times and FLTs information and observations only have times, how does the program know the correspondence of each forecast and observation. It relies on a mapping matrix. The _'real time'_ information of a given forecast is calculated of adding the FLT (think of it like a time offset) to the time (think of it like the initial time of the forecast). And then the _'real time'_ of forecasts is compared to the time of observations to find correspondence when they are the same.

Image the following mapping matrix calculated from the forecast times `(0, 24)` and the forecasts FLTs `(0, 6, 12, 18)`. The cell values are the _'real time'_ of each forecasts by adding times and FLTs.

<table class="tg">
    <tr>
        <th class="tg-uys7"></th>
        <th class="tg-uys7" colspan="2">Time</th>
    </tr>
    <tr>
        <td class="tg-uys7" rowspan="6">FLT</td>
        <td class="tg-uys7">0</td>
        <td class="tg-uys7">24</td>
    </tr>
    <tr>
        <td class="tg-uys7">6</td>
        <td class="tg-uys7">30</td>
    </tr>
    <tr>
        <td class="tg-uys7">12</td>
        <td class="tg-uys7">36</td>
    </tr>
    <tr>
        <td class="tg-uys7">18</td>
        <td class="tg-uys7">42</td>
    </tr>
    <tr>
        <td class="tg-uys7">24</td>
        <td class="tg-uys7">48</td>
    </tr>
    <tr>
        <td class="tg-c3ow">30</td>
        <td class="tg-c3ow">54</td>
    </tr>
</table>

The program will try to find the observations with the same _'real time'_ as a forecast has. This is how the program knows which observation corresponds to which forecast.

## References

- [Cervone, Guido, et al. "Short-term photovoltaic power forecasting using Artificial Neural Networks and an Analog Ensemble." Renewable Energy 108 (2017): 274-286.](https://www.sciencedirect.com/science/article/pii/S0960148117301386)
- [Delle Monache, Luca, et al. "Probabilistic weather prediction with an analog ensemble." Monthly Weather Review 141.10 (2013): 3498-3516.](https://journals.ametsoc.org/doi/abs/10.1175/MWR-D-12-00281.1)
- [Definition of FLT from AMS](http://glossary.ametsoc.org/wiki/Forecast_lead_time)
