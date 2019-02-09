# PEF 3.2.0

- Synchronize versions of all components.
- Add simulation methods: SeparateTestSearch, LeaveOneOut, and OperationalSearch. This has brought an significant addition to similarity calculation function features.

# PEF 3.1.1

## RAnEn

- Refactor Similarity Calculation core function and improve readability.
- Resolve the potential bug in the function `biasCorrectionInsitu`.
- Remove return\_index option from computeSearchStations.
- Skip processing the similarity array if it is not preserved in the result.
- Update documentation.
- Update README file.

# PEF 3.1.0

## CAnEnIO

- Add MPI read functions for values only.
- Fix bug for MPI functions in reading.
- Add serial program limit to AnEn library.
- Add MPI read functions for strings.
- Speed up the station matching process.

## RAnEn

- Reduce code size of the `computeSimilarity` function.
- Resolve a potential bug in `computeSimilarity` and introduce a new parameter `extend_observations` to this function. Now, `extend_observations` is supported by both `computeSimilarity` and `selectAnalogs` functions.
- Minor changes to `RAnEn::biasCorrection`.
- Introduce a new function for bias correction using a neural network.
- Change authors of the package.
- Update documentation.
- Change names of members from bias correction.
- Change import functions to suggested for `parallel`, `nnet`, and `pbapply`.

# PEF 3.0.0

## CAnEnIO

- Basic AnEn I/O functions.

