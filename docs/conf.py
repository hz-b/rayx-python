import importlib.metadata

project = "rayx-python"
author = "RAYX team"
try:
    release = importlib.metadata.version("rayx")
except importlib.metadata.PackageNotFoundError:
    release = "unknown"
version = release

extensions = [
    "myst_parser",
    "sphinx.ext.autodoc",
    "sphinx.ext.autosummary",
    "sphinx.ext.intersphinx",
    "sphinx.ext.napoleon",
    "sphinx.ext.viewcode",
]

source_suffix = {".rst": "restructuredtext", ".md": "markdown"}
master_doc = "index"

autodoc_mock_imports = ["rayx.core"]
autodoc_typehints = "signature"
autodoc_member_order = "bysource"
autosummary_generate = True

napoleon_google_docstring = True
napoleon_numpy_docstring = False

myst_enable_extensions = ["colon_fence", "deflist"]
myst_substitutions = {"release": release}

intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
    "numpy": ("https://numpy.org/doc/stable", None),
    "pandas": ("https://pandas.pydata.org/docs", None),
}

html_theme = "furo"
html_static_path = ["_static"]
