---
layout: post
title: Running Large Scale Analog Ensemble on Cheyenne
tags:
  - showcase
---

<!-- vim-markdown-toc GitLab -->

* [Background](#background)
* [A Brief Introduction to the Problem](#a-brief-introduction-to-the-problem)
* [Workflow](#workflow)

<!-- vim-markdown-toc -->

## Background

This showcase was originally created for the 2019 Software Engineering Assembly (now Improving Scientific Software) conference. During the conference, the presentation was about running large scale Analog Ensemble (AnEn) on NCAR Cheyenne supercomputer systems and the hands-on workshop worked through some basic examples of `RAnEn` locally on a desktop and then showcased the workflow of running large scale AnEn on Cheyenne.

Some helpful information link is provided below:

- [The presentation information page](https://sea.ucar.edu/event/uncertainty-quantification-analog-ensemble-scale)
- [The workshop information page](https://sea.ucar.edu/event/parallel-analog-ensemble-forecasts-ensemble-toolkit-hpc)
- [The slides for the presentation and the workshop](https://prosecco.geog.psu.edu/docs/SEA2019/)

This post summarizes the second part of the workshop, *Analog Ensemble at Scale*.

## A Brief Introduction to the Problem

To generate AnEn for wind speed for one month of July 2018 using 1 year of search data in 2017, since the North America Mesoscale (NAM) model is used, we are dealing with about 838 GB of model data including forecasts and analysis. In total, there are 262,792 grid points in the model domain. This big domain is decomposed (broken) row-wise into 50 chunks so that we can generate AnEn for each chunk of the domain in parallel.

Please find the scripts used in this post [here](https://github.com/Weiming-Hu/AnalogsEnsemble/tree/gh-pages/assets/posts/2019-04-15-large-simulation-on-cheyenne).

Please refer to the [help page on building AnEn on Cheyenne](https://weiming-hu.github.io/AnalogsEnsemble/2019/02/17/build-on-cheyenne.html) if you would like to check out the tools used in this tutorial.

## Workflow

In this workshop, 3 steps are involved for AnEn generation and visualization after you have built/accessed the tools and collected data:

- Step 1: Generate AnEn for each domain chunk. Each domain chunk is associated with a job, and a set of configuration files. Each configuration file specifies to `analogGenerator` which part of the file should be read. A general configuration file is also used to specify some of the common parameters that are shared across all domain chunks like weights and observation id.
- Step 2: Reshape AnEn results from all days per chunk to all chunks per day. It is generally more convenient for verification and visualization when all grid points are include in the same file. This is achieved by reorganizing the data files from separate chunks into a complete domain with day intervals.
- Step 3: Visualize AnEn results. At this point, each NetCDF file should have a daily forecast for the entire model domain which is easy to visualize. An R script is prepared to generate the figures.

This is the video showing how the entire workflow looks like. Click [here](https://youtu.be/GfOmE9zeBLs) if the video is not showing up correctly.

<iframe width="560" height="315" src="https://www.youtube.com/embed/GfOmE9zeBLs" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

Please connect [Weiming Hu](weiming@psu.edu) if you would like a copy of the script.

Thanks.
