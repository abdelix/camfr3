# Sphinx configuration for the camfr3 documentation site (GitHub Pages).
# Build: `make docs` in the repository root (needs the compiled extension,
# which autodoc imports). The Texinfo manual (camfr.texi) is separate.

import re
from importlib.metadata import version as _version

project = "camfr3"
author = "Peter Bienstman and the CAMFR contributors"
copyright = ("1998-2007 Peter Bienstman, Ghent University; "
             "camfr3 maintained by Abdelfettah Hadij-ElHouati")
release = _version("camfr3")
version = release

extensions = [
    "myst_parser",
    "sphinx.ext.autodoc",
    "sphinx.ext.napoleon",
    "sphinx.ext.mathjax",
]

myst_enable_extensions = ["colon_fence", "deflist", "dollarmath"]
myst_heading_anchors = 3

exclude_patterns = ["_build", "figs/*.html", "README.md"]
templates_path = []

html_theme = "furo"
html_title = "camfr3"
html_static_path = ["_static"]
html_css_files = ["custom.css"]
html_theme_options = {
    "source_repository": "https://github.com/abdelix/camfr3/",
    "source_branch": "main",
    "source_directory": "docs/",
}

# Autodoc. The extension is built with pybind11: signatures come from the
# first docstring line, constructors are overloads of __init__.

autodoc_default_options = {"members": True, "undoc-members": False}
autoclass_content = "both"
autodoc_member_order = "alphabetical"
napoleon_google_docstring = True
napoleon_numpy_docstring = True

# pybind11 writes 'typing.SupportsInt | typing.SupportsIndex' for int
# arguments, 'camfr._camfr.Slab' for classes and 'numpy.ndarray[complex128]'
# for mode-amplitude vectors. Shorten them for readability.

_type_subs = [
    (re.compile(r"typing\.SupportsComplex \| typing\.SupportsFloat \| "
                r"typing\.SupportsIndex"), "complex"),
    (re.compile(r"typing\.SupportsComplex \| float"), "complex"),
    (re.compile(r"typing\.SupportsInt \| typing\.SupportsIndex"), "int"),
    (re.compile(r"typing\.SupportsFloat \| typing\.SupportsIndex"), "float"),
    (re.compile(r"camfr\._camfr\."), ""),
    (re.compile(r"numpy\.ndarray\[complex128\]"), "ndarray"),
    (re.compile(r"\bself: \w+(, )?"), ""),
]


def _short(text):
    for pattern, repl in _type_subs:
        text = pattern.sub(repl, text)
    return text


def _signature(app, what, name, obj, options, signature, return_annotation):
    # pybind11 classes and overloaded functions report (*args, **kwargs);
    # their overloads are listed in the text instead.
    if signature == "(*args, **kwargs)":
        return "", None
    if signature is not None:
        signature = _short(signature)
    if return_annotation is not None:
        return_annotation = _short(return_annotation)
    return signature, return_annotation


# Overloaded pybind11 functions have the docstring
#   Overloaded function.
#
#   1. f(a: int) -> None
#
#   Text for overload 1.
#   ...
# Turn each numbered signature into a literal line, so every overload is
# shown with its own text.

_overload = re.compile(r"^(\d+)\. (\S.*)$")


def _docstring(app, what, name, obj, options, lines):
    if not lines or lines[0].strip() != "Overloaded function.":
        for i, line in enumerate(lines):
            lines[i] = _short(line)
        return
    out = []
    for line in lines[1:]:
        m = _overload.match(line)
        if m:
            sig = _short(m.group(2))
            out.append(f"``{sig}``")
        else:
            out.append(_short(line))
    lines[:] = out


def setup(app):
    app.connect("autodoc-process-signature", _signature)
    app.connect("autodoc-process-docstring", _docstring)
