# RAnEn 3.1.1

- Refactor Simialrity Calculation core function and improve readability.
- Resolve the potential bug in in situ bias correction function.
- Update documentation.
- Update README file.

# RAnEn 3.1.0

- Reduce code size of the `computeSimilarity` function.
- Resolve a potential bug in `computeSimilarity` and introduce a new parameter `extend_observations` to this function. Now, `extend_observations` is supported by both `computeSimilarity` and `selectAnalogs` functions.
- Minor changes to `RAnEn::biasCorrection`.
- Introduce a new function for bias correction using a neural network.
- Change authors of the package.
- Update documentation.
- Change names of members from bias correction.
- Change import functions to suggested for `parallel`, `nnet`, and `pbapply`.