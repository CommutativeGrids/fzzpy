#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "fzz.h"

namespace py = pybind11;

std::vector< std::tuple<FZZ::Integer, FZZ::Integer, FZZ::Integer> > py_compute(
    const std::vector<FZZ::Simplex>& filt_simp,
    const std::vector<bool>& filt_op
) {
    FZZ::FastZigzag fzz;
    std::vector< std::tuple<FZZ::Integer, FZZ::Integer, FZZ::Integer> > persistence;
    fzz.compute(filt_simp, filt_op, &persistence);
    return persistence;
}

PYBIND11_MODULE(fzz_module, m) {
    m.doc() = "Modified Python wrapper for FastZigzag class from fzz C++ library";

    m.def("compute", &py_compute, "Compute function for FastZigzag",
          py::arg("filt_simp"), py::arg("filt_op"));
}
