/** \file telink_python.cxx
 *  Python wrapper for C++ classes.
 *  Author: Vincent Paeder
 *  License: GPL v3
 */
#include "telink_python.h"

namespace boost { namespace python {
    bool hasattr(PyObject * o, const char* name) {
      handle<> handle(borrowed(o));
      object oo(handle);
      return PyObject_HasAttrString(oo.ptr(), name);
    }
}}

namespace telink {
  
  static void call_python_callback(PyObject * o, const char * method_name, const std::string & arg) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    if (bp::hasattr(o, method_name))
      bp::call_method<void>(o, method_name, arg);
    PyGILState_Release(gstate);
  }
  
  void TelinkLightPython::set_alarm(unsigned char alarm_id, bp::list & list_weekdays, unsigned char hour, unsigned char minute, unsigned char second, unsigned char action) {
    std::vector<bool> weekdays(7);
    for (int i=0; i<7; i++)
      weekdays.push_back(bp::extract<bool>(list_weekdays[i]));
  
    TelinkLight::set_alarm(alarm_id, weekdays, hour, minute, second, action);
  }

  void TelinkLightPython::set_alarm(unsigned char alarm_id, bool state) {
    TelinkLight::set_alarm(alarm_id, state);
  }

  void TelinkLightPythonCallback::parse_online_status_report(const std::string & packet) {
    TelinkLightPython::parse_online_status_report(packet);
    call_python_callback(self, "parse_online_status_report", packet);
  }

  void TelinkLightPythonCallback::parse_status_report(const std::string & packet) {
    TelinkLight::parse_status_report(packet);
    call_python_callback(self, "parse_status_report", packet);
  }

  void TelinkLightPythonCallback::parse_alarm_report(const std::string & packet) {
    TelinkLight::parse_alarm_report(packet);
    call_python_callback(self, "parse_alarm_report", packet);
  }

  void TelinkLightPythonCallback::parse_scenario_report(const std::string & packet) {
    TelinkLight::parse_scenario_report(packet);
    call_python_callback(self, "parse_scenario_report", packet);
  }

  void TelinkMeshPythonCallback::parse_time_report(const std::string & packet) {
    TelinkMesh::parse_time_report(packet);
    call_python_callback(self, "parse_time_report", packet);
  }

  void TelinkMeshPythonCallback::parse_address_report(const std::string & packet) {
    TelinkMesh::parse_address_report(packet);
    call_python_callback(self, "parse_address_report", packet);
  }

  void TelinkMeshPythonCallback::parse_device_info_report(const std::string & packet) {
    TelinkMesh::parse_device_info_report(packet);
    call_python_callback(self, "parse_device_info_report", packet);
  }

  void TelinkMeshPythonCallback::parse_group_id_report(const std::string & packet) {
    TelinkMesh::parse_group_id_report(packet);
    call_python_callback(self, "parse_group_id_report", packet);
  }
  
  // Python classes definitions - module will be called telink_wrapper.so
  BOOST_PYTHON_MODULE(telink_wrapper) {
    
    // TelinkMesh
    bp::class_<TelinkMesh, boost::noncopyable>("TelinkMesh", bp::no_init)
      .def(bp::init<std::string>())
      .def(bp::init<std::string, std::string, std::string>())
      .def("set_address", &TelinkMesh::set_address)
      .def("set_name", &TelinkMesh::set_name)
      .def("set_password", &TelinkMesh::set_password)
      .def("set_vendor", &TelinkMesh::set_vendor)
      .def("set_mesh_id", &TelinkMesh::set_mesh_id)
      .def("send_packet", &TelinkMesh::send_packet)
      .def("connect", &TelinkMesh::connect)
      .def("disconnect", &TelinkMesh::disconnect)
      .def("is_connected", &TelinkMesh::is_connected);
  
    // TelinkColor
    void (TelinkColor::*set_temperature)(int) = &TelinkColor::set_temperature;
    void (TelinkColor::*set_temperature_WY)(unsigned char, unsigned char) = &TelinkColor::set_temperature;
    bp::class_<TelinkColor, boost::noncopyable>("TelinkColor", bp::no_init)
      .def(bp::init<int, int, int, int>())
      .def(bp::init<int, int, int>())
      .def(bp::init<int, int>())
      .def("set_brightness", &TelinkColor::set_brightness)
      .def("set_color", &TelinkColor::set_color)
      .def("set_temperature", set_temperature)
      .def("set_temperature", set_temperature_WY)
      .def("get_bytes", &TelinkColor::get_bytes);
    
    // TelinkScenario
    void (TelinkScenario::*add_color_speed)(TelinkColor, unsigned char) = &TelinkScenario::add_color;
    void (TelinkScenario::*add_color)(TelinkColor) = &TelinkScenario::add_color;
    bp::class_<TelinkScenario, boost::noncopyable>("TelinkScenario")
      .def("add_color", add_color_speed)
      .def("add_color", add_color)
      .def("replace_color", &TelinkScenario::replace_color)
      .def("remove_color", &TelinkScenario::remove_color)
      .def("set_speed", &TelinkScenario::set_speed)
      .def("get_size", &TelinkScenario::get_size)
      .def("get_bytes", &TelinkScenario::get_bytes);
    
    // TelinkLight
    void (TelinkLightPython::*set_alarm)(unsigned char, bp::list&, unsigned char, unsigned char, unsigned char, unsigned char) = &TelinkLightPython::set_alarm;
    void (TelinkLightPython::*set_alarm_on_off)(unsigned char, bool) = &TelinkLightPython::set_alarm;
    bp::class_<TelinkLightPython, TelinkLightPythonCallback, boost::noncopyable>("TelinkLight", bp::no_init)
      .def(bp::init<std::string, std::string, std::string>())
      .def("query_groups", &TelinkLightPython::query_groups)
      .def("set_time", &TelinkLightPython::set_time)
      .def("query_time", &TelinkLightPython::query_time)
      .def("query_alarm", &TelinkLightPython::query_alarm)
      .def("query_device_info", &TelinkLightPython::query_device_info)
      .def("query_device_version", &TelinkLightPython::query_device_version)
      .def("query_scenario", &TelinkLightPython::query_scenario)
      .def("query_status", &TelinkLightPython::query_status)
      .def("query_mesh_id", &TelinkLightPython::query_mesh_id)
      .def("set_state", &TelinkLightPython::set_state)
      .def("set_mesh_id", &TelinkLightPython::set_mesh_id)
      .def("add_group", &TelinkLightPython::add_group)
      .def("delete_group", &TelinkLightPython::delete_group)
      .def("add_scenario", &TelinkLightPython::add_scenario)
      .def("delete_scenario", &TelinkLightPython::delete_scenario)
      .def("set_color", &TelinkLightPython::set_color)
      .def("set_temperature", &TelinkLightPython::set_temperature)
      .def("set_music_mode", &TelinkLightPython::set_music_mode)
      .def("load_scenario", &TelinkLightPython::load_scenario)
      .def("set_alarm", set_alarm)
      .def("set_alarm", set_alarm_on_off)
      .def("delete_alarm", &TelinkLightPython::delete_alarm)
      .def("edit_scenario", &TelinkLightPython::edit_scenario)
  		.def("set_address", &TelinkLightPython::set_address)
    	.def("set_name", &TelinkLightPython::set_name)
      .def("set_password", &TelinkLightPython::set_password)
      .def("set_vendor", &TelinkLightPython::set_vendor)
      .def("set_mesh_id", &TelinkLightPython::set_mesh_id)
      .def("send_packet", &TelinkLightPython::send_packet)
      .def("connect", &TelinkLightPython::connect)
      .def("disconnect", &TelinkLightPython::disconnect)
      .def("is_connected", &TelinkLightPython::is_connected);
    
  }
}
