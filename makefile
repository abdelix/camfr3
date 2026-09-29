camfr: FORCE
	python3 -m SCons

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
	scons -c
	cd examples ; make clean
	cd visualisation ; make clean
	cd testsuite ; make clean
	cd docs ; make clean
