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

test: FORCE
	cd testsuite ; make

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
