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

The prediction accuracy of Analog Ensemble depends on the quality of analogs. Presumably, better analogs will generate better predictions. In real-time model operation, when historical forecasts increasing and the test forecast moving forward in time, it is most likely that the historical forecasts in the near past are the most similar ones. Therefore, in an operational prediction mode, it is desired to increment the search forecasts when test forecasts are moving towards the future.

This article shows an example of how to use `RAnEn` with an operational search. It is strongly suggested to go over the [demo 1](https://weiming-hu.github.io/AnalogsEnsemble/2018/11/04/demo-1-RAnEn-basics.html) prior to this tutorial.

Access
------------

This tutorial can now be accessed on binder. Please click [here](https://mybinder.org/v2/gh/Weiming-Hu/AnalogsEnsemble/master) to start an interactive session and go over the tutorial in a python notebook.

Or you can download the repository and use the [R markdown file](https://github.com/Weiming-Hu/AnalogsEnsemble/blob/master/RAnalogs/examples/demo-3_operational-search.Rmd).
