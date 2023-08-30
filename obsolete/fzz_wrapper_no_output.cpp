
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "fzz.h"

namespace py = pybind11;

void py_compute(
    const std::vector<FZZ::Simplex>& filt_simp,
    const std::vector<bool>& filt_op,
    std::vector< std::tuple<FZZ::Integer, FZZ::Integer, FZZ::Integer> >& persistence
) {
    FZZ::FastZigzag fzz;
    fzz.compute(filt_simp, filt_op, &persistence);
}

PYBIND11_MODULE(fzz_module, m) {
    m.doc() = "Python wrapper for FastZigzag class from fzz C++ library";

    m.def("compute", &py_compute, "Compute function for FastZigzag",
          py::arg("filt_simp"), py::arg("filt_op"), py::arg("persistence"));
}
