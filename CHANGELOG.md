# CHANGELOG

<!-- version list -->

## v3.0.0 (2026-10-01)

First stable release of camfr3, the Python 3 port of CAMFR: `pip install camfr3`
installs it without `--pre`. Same code as v3.0.0-alpha.4, plus the release
configuration (no more alpha pre-releases; Development Status: Beta).

Since the original CAMFR (Python 2, SCons, Boost.Python), see the alpha releases
below and PORTING_JOURNAL.md: Python 3 (3.10-3.14), CMake + scikit-build-core,
pybind11 bindings with correct object lifetimes, memory-safety fixes found with
ASan/UBSan, a pytest testsuite run in CI, Linux wheels on PyPI, a documentation
site (https://abdelix.com/camfr3/), and no code under ACM's non-commercial
licence (Jenkins-Traub replaced; Patterson quadrature based on JPL MATH77).

## v3.0.0-alpha.4 (2026-10-01)

### Bug Fixes

- **quadrature**: Rebase the Patterson quadrature on MATH77 (BSD licence)
  ([`72f2393`](https://github.com/abdelix/camfr3/commit/72f23939c7301a9965bc65d46746cda06353b837))

### Build System

- Publish releases to PyPI with trusted publishing
  ([`3251e71`](https://github.com/abdelix/camfr3/commit/3251e713a34eea651890164791bd881f4928a831))

### Continuous Integration

- Publish an existing release tag to PyPI on a manual run
  ([`4cb46bc`](https://github.com/abdelix/camfr3/commit/4cb46bc082a0f1620f6761388448166e5ed612a8))

### Documentation

- Describe camfr3 as maintained in my free time, in the first person
  ([`f58eda6`](https://github.com/abdelix/camfr3/commit/f58eda655b33962b7c048fc97890a4621e3bea4a))

- **notice**: Licences of the vendored gifmaker and TkPlotCanvas modules
  ([`2621d0a`](https://github.com/abdelix/camfr3/commit/2621d0a84b58e2a2120192ad53cb56bfb3a4a4c3))


## v3.0.0-alpha.3 (2026-09-30)

### Bug Fixes

- **plot**: Cavity.plot raised NameError since the lazy plot imports
  ([`3a9d17b`](https://github.com/abdelix/camfr3/commit/3a9d17b8bb45a2c143597a185bc6c4871023861d))

### Build System

- Build and test Linux wheels with cibuildwheel
  ([`cfd959c`](https://github.com/abdelix/camfr3/commit/cfd959cf323667305b7ef2b6165b7c96a26c51e0))

- Replace MACHAR with the language's machine constants
  ([`3f8ded3`](https://github.com/abdelix/camfr3/commit/3f8ded3c995194ec857025d1182938628bada682))

### Documentation

- Documentation site with Sphinx, deployed to GitHub Pages
  ([`d2b7bb3`](https://github.com/abdelix/camfr3/commit/d2b7bb321241059eea931350115116a976f33db1))

- Name the maintainer in the README
  ([`eb6ccf2`](https://github.com/abdelix/camfr3/commit/eb6ccf26598d9692d042224b5b92d6e8539d5736))

- The site's address is abdelix.com/camfr3 (custom domain)
  ([`f6f7832`](https://github.com/abdelix/camfr3/commit/f6f7832ce901a94eb12447f0d08a3a00b0d02cb4))

- **wrap**: Docstrings and argument names for the bindings
  ([`9641145`](https://github.com/abdelix/camfr3/commit/96411455df2602b9aafbb72203112a0ae1b71b6e))


## v3.0.0-alpha.2 (2026-09-30)

### Bug Fixes

- **croot**: Do not read past the moments in the contour root finder
  ([`bfbe051`](https://github.com/abdelix/camfr3/commit/bfbe0518f12432db8e2e2b80944c6c9f6bd650e1))

- **polyroot**: Copy the roots before freeing them
  ([`aea2d31`](https://github.com/abdelix/camfr3/commit/aea2d312f6cbd0390e78235f8a53a7dc8df8541e))

- **stack**: Do not read past the last term when building chunks
  ([`8224a19`](https://github.com/abdelix/camfr3/commit/8224a194cafac7ffcd860c6a63913753af278755))

- **stack**: Initialise bw_inc and copy it with the stack
  ([`2556272`](https://github.com/abdelix/camfr3/commit/255627220caf3e6251427e2fc779f96b4e2ecdd8))

- **visualisation**: Use a raw string for the matplotlib mode label
  ([`6a16480`](https://github.com/abdelix/camfr3/commit/6a16480411386fbdd21d3d5f99ef19676217a1a2))

- **wrap**: Keep Python objects alive while C++ points to them
  ([`0150c81`](https://github.com/abdelix/camfr3/commit/0150c8161cda9a017f0cfde29265da96df6c065d))

- **wrap**: Make str() of enum values the bare name again
  ([`b8d1968`](https://github.com/abdelix/camfr3/commit/b8d1968ff4f30cd2c56140f9f1202cda72a66f1f))

### Build System

- Enable compiler warnings and add an ASan/UBSan build
  ([`decf9d4`](https://github.com/abdelix/camfr3/commit/decf9d4264ca40609fa448c388ccbc4fb1469e09))

- Make Matplotlib and Pillow required dependencies again
  ([`8fcbba0`](https://github.com/abdelix/camfr3/commit/8fcbba0cdbd65f77787dcc1442f16484a79d09a5))

- Manage the development environment with uv
  ([`a89880d`](https://github.com/abdelix/camfr3/commit/a89880d9cd28d4b190dca0c6d1e746a7042fc71c))

- Remove the Python 2-era machine_cfg templates
  ([`a96b14f`](https://github.com/abdelix/camfr3/commit/a96b14f78b585e1e1ff2e1bc36359f9b3387b52c))

- Replace SCons with CMake and scikit-build-core
  ([`eebfd05`](https://github.com/abdelix/camfr3/commit/eebfd05e40859365acd7be7af1ebac40ab589f1c))

### Continuous Integration

- Add GitHub Actions for tests and a sanitizer run
  ([`401f23b`](https://github.com/abdelix/camfr3/commit/401f23bcf11ae01537a6b89b321f6c61e4ca3398))

- Create releases automatically with python-semantic-release
  ([`f4777f6`](https://github.com/abdelix/camfr3/commit/f4777f6666a312b6daa739d55df02368dc634c26))

- Move to Node 24 versions of checkout and setup-uv
  ([`1ddb06f`](https://github.com/abdelix/camfr3/commit/1ddb06fd909b3d3b278a9ba8c51872b61d2c98be))

### Documentation

- Record that origin now uses SSH
  ([`55e437a`](https://github.com/abdelix/camfr3/commit/55e437ad140c450c3f31f311ba645a30b43236f6))

- Record the fork as origin and the pushed v3.0.0a1 tag
  ([`89b6c2f`](https://github.com/abdelix/camfr3/commit/89b6c2fdd6e2de3209af2fd4e50dca259a5b7628))

- Record the project goal: revive CAMFR as a side project
  ([`beb1344`](https://github.com/abdelix/camfr3/commit/beb1344956a330d2dcaee2f4aece5f3dc652a733))

- Record the second-Section segfault investigation
  ([`ecd79fd`](https://github.com/abdelix/camfr3/commit/ecd79fde452181fee6d79c451d5c14a5cb48f737))

- Record the split into python3-port and modernisation branches
  ([`8304614`](https://github.com/abdelix/camfr3/commit/8304614ab1b70ece5343cfcb8158b15485feacc2))

- Use the renamed fork abdelix/camfr3 and record how to push
  ([`0346d68`](https://github.com/abdelix/camfr3/commit/0346d6809ee2823aac0fb1cdbfc736f2f79fe780))

### Features

- Import plotting lazily and make Matplotlib and Pillow optional
  ([`2fbe5d3`](https://github.com/abdelix/camfr3/commit/2fbe5d3c718911772a711b26b12e88fade4a0adc))

- **polyroot**: Use the companion matrix instead of Jenkins-Traub
  ([`32aa8db`](https://github.com/abdelix/camfr3/commit/32aa8dbe27aaab8c5369832ed656c008569da9c5))

- **wrap**: Port the Python bindings from Boost.Python to pybind11
  ([`9339b4d`](https://github.com/abdelix/camfr3/commit/9339b4dd59fe250e90251b567963cc4c42aa44a8))

### Testing

- Check the metal_splitter slab modes against Palace
  ([`6e437fa`](https://github.com/abdelix/camfr3/commit/6e437fa76eb9199d68f784d146a666c04dec9534))

- Re-enable ADR_solver and isolate tests from leftover settings
  ([`976c6e5`](https://github.com/abdelix/camfr3/commit/976c6e5366d47e13b09ad8335c1cd5f46dc6caea))

- Run the testsuite with pytest, one process per test
  ([`66c3e44`](https://github.com/abdelix/camfr3/commit/66c3e44338efe088f0413f733a6bf0f47662f56e))


## v3.0.0-alpha.1 (2026-09-30)

- Initial Release
