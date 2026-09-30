# pytest configuration for the CAMFR testsuite.
#
# The test modules are plain unittest files (TestCase classes), shared with the
# camfr_test.py runner. They import the helper module 'eps' directly, so this
# directory goes on sys.path; with --import-mode=importlib the repository root
# does not, so 'import camfr' finds the installed package, not the sources.

import os
import sys

import pytest

os.environ.setdefault("MPLBACKEND", "Agg")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Not test modules: the unittest runner (it imports all tests again) and helpers.

collect_ignore = ["camfr_test.py", "eps.py", "__init__.py"]

# Known failures (PORTING_JOURNAL.md entries 45-46). strict: an unexpected pass
# fails the run, so the marker is removed when a test starts to pass. Only the
# known wrong result (AssertionError) is expected; any other error still fails.

XFAIL = {
    "stack2": "ill-conditioned: R12 does not converge with N (journal entry 45)",
    "metal_splitter": "slab modes need low_index_core, and R12 stays unphysical "
                      "with complete modes (journal entries 45-46)",
}

def pytest_collection_modifyitems(config, items):
    for item in items:
        module = os.path.splitext(os.path.basename(str(item.fspath)))[0]
        if module in XFAIL:
            item.add_marker(pytest.mark.xfail(reason=XFAIL[module], strict=True,
                                              raises=AssertionError))
