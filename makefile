# Convenience targets. The build itself is CMake via scikit-build-core
# (pyproject.toml, CMakeLists.txt).

camfr: FORCE
	uv sync

# Incremental rebuild in .venv after C++/Fortran edits (~7 s instead of a full
# rebuild): builds without isolation, with the tools from the uv dev group.
dev: FORCE
	uv pip install --no-build-isolation --no-deps -e .

install:
	python3 -m pip install .

# AddressSanitizer + UBSan build (RelWithDebInfo, own build directory),
# installed into .venv in place of the normal one; 'make dev' switches back.
# Run Python with the ASan runtime preloaded, e.g.
#   LD_PRELOAD=$(gcc -print-file-name=libasan.so) ASAN_OPTIONS=detect_leaks=0 \
#   UBSAN_OPTIONS=print_stacktrace=1:suppressions=$PWD/ubsan.supp \
#     .venv/bin/python script.py
asan: FORCE
	uv pip install --no-build-isolation --no-deps -e . \
	  -C cmake.define.CAMFR_SANITIZE=ON -C cmake.build-type=RelWithDebInfo \
	  -C build-dir=build/asan

test: FORCE
	cd testsuite ; make

# The documentation site (docs/, Sphinx), built into docs/_build/html. -W:
# warnings are errors, as in CI.
docs: FORCE
	uv run --group docs sphinx-build -W --keep-going -b html docs docs/_build/html

FORCE:

clean:
	rm -f *~ *.pyc core MANIFEST
	rm -f -R *.egg-info
	rm -f -R build
	rm -f -R dist
	cd examples ; make clean
	cd visualisation ; make clean
	cd testsuite ; make clean
	cd docs ; make clean
