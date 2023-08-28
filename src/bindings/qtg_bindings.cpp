#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

#include "generator/jooken_generator.h"
#include "common/utils.hpp"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<utils::cpp_item>);


PYBIND11_MODULE(_qtg_bindings, m
) {
    m.def("jooken_generate", &generator::generate);

    py::class_<std::vector<utils::cpp_item>>(m, "ItemVector")
            .def(py::init<>())
            .def("clear", &std::vector<utils::cpp_item>::clear)
            .def("pop_back", &std::vector<utils::cpp_item>::pop_back)
            .def("__len__", [](const std::vector<utils::cpp_item> &v) { return v.size(); })
            .def("__iter__", [](std::vector<utils::cpp_item> &v) {
                return py::make_iterator(v.begin(), v.end());
            }, py::keep_alive<0, 1>());

    py::class_<utils::cpp_knapsack>(m, "Knapsack")
            .def_readwrite("size", &utils::cpp_knapsack::size)
            .def_readwrite("capacity", &utils::cpp_knapsack::capacity)
            .def_readwrite("items", &utils::cpp_knapsack::items)
            .def_readwrite("name", &utils::cpp_knapsack::name);

    py::class_<utils::cpp_item>(m, "Item")
            .def_readwrite("profit", &utils::cpp_item::profit)
            .def_readwrite("cost", &utils::cpp_item::cost);
}