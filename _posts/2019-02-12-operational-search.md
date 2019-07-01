---
layout: post
title: Operational Search with RAnEn
tags:
  - tutorial
---

<!-- vim-markdown-toc GitLab -->

* [Introduction](#introduction)
* [Access](#access)

<!-- vim-markdown-toc -->

Introduction
------------

Prediction accuracy of the Analog Ensemble depends on the quality of analogs. Presumably, better analogs will generate better predictions. In an operational model, it is likely that the historical forecasts in the near past are the most similar to the current forecast. Therefore, in operational mode, as each day passes, it is added to the historical repository.

This article shows an example of how to use `RAnEn` with an operational search. It is strongly suggested to go over the [demo 1](https://weiming-hu.github.io/AnalogsEnsemble/2018/11/04/demo-1-RAnEn-basics.html) prior to this tutorial.

Access
------------

This tutorial can be accessed on binder. Please click [here](https://mybinder.org/v2/gh/Weiming-Hu/AnalogsEnsemble/master) to start an interactive session and go over the tutorial in a python notebook. Or you can download the repository and use the [R markdown file](https://github.com/Weiming-Hu/AnalogsEnsemble/blob/master/RAnalogs/examples/demo-3_operational-search.Rmd).
