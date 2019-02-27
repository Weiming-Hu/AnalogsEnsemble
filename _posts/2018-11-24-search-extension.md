---
layout: post
title: Search Space Extension with RAnEn
tags:
  - tutorial
---

<!-- vim-markdown-toc GitLab -->

* [Introduction](#introduction)
* [Access](#access)

<!-- vim-markdown-toc -->

Introduction
------------

This article demonstrate how to use the `RAnEn` package with the search extension functionality. If you haven't done so, please read [the instructions for basic usage of `RAnEn`](https://weiming-hu.github.io/AnalogsEnsemble/2018/11/04/demo-1-RAnEn-basics.html) first. This article skips the part that has been covered in the previous article.

The classic `AnEn` algorithm searches for the most similar historical foreasts at its current location. Therefore, only forecasts from the current station/grid point will be traversed and compared. This search style is referred to as the *independent search*. Another possible search style is *extended search*, which is also referred to as *search space extension*. It simply indicates that forecasts at nearby stations/grid points are included in the search process. As a result, the search space is significantly larger by using the search space extension.

There are currently two ways to define what nearby locations to be included into the search. Users can set the nearest number of neighbors to be included or a distance threshold. Two restraints can be used together.

You will learn how to use these functions:

-   `generateAnalogs`

Access
------------

This tutorial can now be accessed on binder. Please click [here](https://mybinder.org/v2/gh/Weiming-Hu/AnalogsEnsemble/master) to start an interactive session and go over the tutorial in a python notebook.

Or you can download the repository and use the [R markdown file](https://github.com/Weiming-Hu/AnalogsEnsemble/blob/master/RAnalogs/examples/demo-2_search-extension.Rmd).