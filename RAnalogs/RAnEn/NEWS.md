# RAnEn 3.1.1

- Refactor Similarity Calculation core function and improve readability.
- Resolve the potential bug in the function `biasCorrectionInsitu`.
- Remove return\_index option from computeSearchStations.
- Skip processing the similarity array if it is not preserved in the result.
- Update documentation.
- Update README file.
- Add function RAnEn::version.

# RAnEn 3.1.0

- Reduce code size of the `computeSimilarity` function.
- Resolve a potential bug in `computeSimilarity` and introduce a new parameter `extend_observations` to this function. Now, `extend_observations` is supported by both `computeSimilarity` and `selectAnalogs` functions.
- Minor changes to `RAnEn::biasCorrection`.
- Introduce a new function for bias correction using a neural network.
- Change authors of the package.
- Update documentation.
- Change names of members from bias correction.
- Change import functions to suggested for `parallel`, `nnet`, and `pbapply`.
