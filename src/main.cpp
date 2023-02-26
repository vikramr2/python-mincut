#include <pybind11/pybind11/h>

namespace py = pybind11;

float mincut(float arg1, float arg2) {
    return arg1 + arg2;
}

PYBIND11_MODULE(mincut, handle) {
    handle.doc() = "This is the module docs.";
    handle.def("mincut", &mincut);
}