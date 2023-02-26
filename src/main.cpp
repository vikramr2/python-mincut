#include <pybind11/pybind11.h>

namespace py = pybind11;

float mincut(float arg1, float arg2) {
    return arg1 + arg2;
}

class Graph {
    float multiplier;

public:
    Graph(float multiplier_) : multiplier(multiplier_) {};
    float multiply(float input) {
        return multiplier * input;
    }
};

PYBIND11_MODULE(mincut, handle) {
    handle.doc() = "This is the module docs.";
    handle.def("mincut", &mincut);

    py::class_<Graph>(
        handle, "PyGraph"
    )
    .def(py::init<float>())
    .def("multiply", &Graph::multiply);
}