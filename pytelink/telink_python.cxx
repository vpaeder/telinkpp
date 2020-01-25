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
    
    bp::docstring_options doc_options;
    doc_options.enable_user_defined();
    doc_options.enable_py_signatures();
    doc_options.disable_cpp_signatures();
    
    // TelinkMesh
    bp::class_<TelinkMesh, boost::noncopyable>("TelinkMesh", "Class handling connection with a Bluetooth LE device with Telink mesh protocol.", bp::no_init)
      .def(bp::init<std::string>((bp::arg("address"))))
      .def(bp::init<std::string, std::string, std::string>((bp::arg("address"), bp::arg("name"), bp::arg("password"))))
      .def("set_address", &TelinkMesh::set_address, bp::args("address"), "Sets the MAC address to connect to.")
      .def("set_name", &TelinkMesh::set_name, bp::args("name"), "Sets the device name to be used for connecting.")
      .def("set_password", &TelinkMesh::set_password, bp::args("password"), "Sets the password to be used for connecting.")
      .def("set_vendor", &TelinkMesh::set_vendor, bp::args("vendor"), "Sets the Bluetooth vendor code (0x0211 for Telink).")
      .def("query_mesh_id", &TelinkLightPython::query_mesh_id, "Queries mesh ID from device.")
      .def("set_mesh_id", &TelinkMesh::set_mesh_id, bp::args("mesh_id"), "Sets device mesh ID.")
      .def("send_packet", &TelinkMesh::send_packet, bp::args("command", "data"), "Sends a command packet to the device.")
      .def("connect", &TelinkMesh::connect, "Connects to Bluetooth device.")
      .def("disconnect", &TelinkMesh::disconnect, "Disconnects from Bluetooth device.")
      .def("is_connected", &TelinkMesh::is_connected, "Probes whether the connection with the device is established.")
      .def("query_groups", &TelinkLightPython::query_groups, "Queries mesh group IDs from device.")
      .def("add_group", &TelinkMesh::add_group, bp::args("group_id"), "Adds device to given group.")
      .def("delete_group", &TelinkMesh::delete_group, bp::args("group_id"), "Removes device from given group.")
      .def("set_time", &TelinkLightPython::set_time, "Sets device date and time.")
      .def("query_time", &TelinkLightPython::query_time, "Queries device date and time.")
      .def("query_device_info", &TelinkLightPython::query_device_info, "Queries device information.")
      .def("query_device_version", &TelinkLightPython::query_device_version, "Queries device firmware version.");
  
    // TelinkColor
    void (TelinkColor::*set_temperature)(int) = &TelinkColor::set_temperature;
    void (TelinkColor::*set_temperature_WY)(unsigned char, unsigned char) = &TelinkColor::set_temperature;
    bp::class_<TelinkColor, boost::noncopyable>("TelinkColor", "Class representing a color state.", bp::no_init)
      .def(bp::init<int, int, int, int>((bp::arg("R"),bp::arg("G"),bp::arg("B"),bp::arg("brightness"))))
      .def(bp::init<int, int, int>((bp::arg("Y"),bp::arg("W"),bp::arg("brightness"))))
      .def(bp::init<int, int>((bp::arg("temperature"),bp::arg("brightness"))))
      .def("set_brightness", &TelinkColor::set_brightness, bp::args("brightness"), "Sets the color brightness.")
      .def("set_color", &TelinkColor::set_color, bp::args("R", "G", "B"), "Sets the RGB color.")
      .def("set_temperature", set_temperature, bp::args("temperature"), "Sets the color temperature using equivalent black body temperature.")
      .def("set_temperature", set_temperature_WY, bp::args("W", "Y"), "Sets the color temperature using CCT Y and W parameters.")
      .def("get_bytes", &TelinkColor::get_bytes, "Compiles a byte string to be sent as data to the device.");
    
    // TelinkScenario
    void (TelinkScenario::*add_color_speed)(TelinkColor, unsigned char) = &TelinkScenario::add_color;
    void (TelinkScenario::*add_color)(TelinkColor) = &TelinkScenario::add_color;
    bp::class_<TelinkScenario, boost::noncopyable>("TelinkScenario", "Class representing a scenario.")
      .def("add_color", add_color_speed, bp::args("color", "speed"), "Adds a color to the list.")
      .def("add_color", add_color, bp::args("color"), "Adds a color to the list.")
      .def("replace_color", &TelinkScenario::replace_color, bp::args("color_index", "color"), "Replace a color in the list.")
      .def("remove_color", &TelinkScenario::remove_color, bp::args("color_index"), "Removes a color from the list.")
      .def("set_speed", &TelinkScenario::set_speed, bp::args("color_index", "speed"), "Sets the speed for the given color.")
      .def("get_size", &TelinkScenario::get_size, "Returns the size of the color vector.")
      .def("get_bytes", &TelinkScenario::get_bytes, bp::args("color_index"), "Compiles a byte string to be sent as data to the device.");
    
    // TelinkLight
    void (TelinkLightPython::*set_alarm)(unsigned char, bp::list&, unsigned char, unsigned char, unsigned char, unsigned char) = &TelinkLightPython::set_alarm;
    void (TelinkLightPython::*set_alarm_on_off)(unsigned char, bool) = &TelinkLightPython::set_alarm;
    bp::class_<TelinkLightPython, TelinkLightPythonCallback, boost::noncopyable>("TelinkLight", bp::no_init)
      .def(bp::init<std::string, std::string, std::string>((bp::arg("address"), bp::arg("name"), bp::arg("password"))))
      .def("query_groups", &TelinkLightPython::query_groups, "Queries mesh group IDs from device.")
      .def("add_group", &TelinkMesh::add_group, bp::args("group_id"), "Queries mesh group IDs from device.")
      .def("delete_group", &TelinkMesh::delete_group, bp::args("group_id"), "Removes device from given group.")
      .def("set_time", &TelinkLightPython::set_time, "Sets device date and time.")
      .def("query_time", &TelinkLightPython::query_time, "Queries device date and time.")
      .def("query_alarm", &TelinkLightPython::query_alarm, "Queries alarm status from device.")
      .def("query_device_info", &TelinkLightPython::query_device_info, "Queries device information.")
      .def("query_device_version", &TelinkLightPython::query_device_version, "Queries device firmware version.")
      .def("query_scenario", &TelinkLightPython::query_scenario, "Queries scenario details from device.")
      .def("query_status", &TelinkLightPython::query_status, "Queries device status.")
      .def("query_mesh_id", &TelinkLightPython::query_mesh_id, "Queries mesh ID from device.")
      .def("set_state", &TelinkLightPython::set_state, bp::args("state"), "Sets device power state.")
      .def("set_mesh_id", &TelinkLightPython::set_mesh_id, bp::args("mesh_id"), "Sets device mesh ID.")
      .def("add_group", &TelinkLightPython::add_group, bp::args("group_id"), "Adds device to given group.")
      .def("delete_group", &TelinkLightPython::delete_group, bp::args("group_id"), "Removes device from given group.")
      .def("add_scenario", &TelinkLightPython::add_scenario, bp::args("scenario_id"), "Adds given scenario to device.")
      .def("delete_scenario", &TelinkLightPython::delete_scenario, bp::args("scenario_id"), "Deletes given scenario from device.")
      .def("set_brightness", &TelinkLightPython::set_brightness, bp::args("brightness"), "Sets light brightness.")
      .def("set_color", &TelinkLightPython::set_color, bp::args("R", "G", "B"), "Sets light RGB color.")
      .def("set_temperature", &TelinkLightPython::set_temperature, bp::args("temperature"), "Sets light color temperature.")
      .def("set_music_mode", &TelinkLightPython::set_music_mode, bp::args("music_mode"), "Sets device music mode: color/brightness changes are faster, but aren't acknowledged by replies.")
      .def("load_scenario", &TelinkLightPython::load_scenario, bp::args("scenario_id", "speed"), "Loads the scenario with given scenario ID on device.")
      .def("set_alarm", set_alarm, bp::args("alarm_id", "weekdays", "hour", "minute", "second", "action"), "Sets an alarm with given parameters.")
      .def("set_alarm", set_alarm_on_off, bp::args("alarm_id", "state"), "Changes the state of an alarm.")
      .def("delete_alarm", &TelinkLightPython::delete_alarm, bp::args("alarm_id"), "Deletes an alarm.")
      .def("edit_scenario", &TelinkLightPython::edit_scenario, bp::args("scenario_id", "scenario"), "Edits a light scenario. A light scenario is a series of colors that are cycled through.")
  		.def("set_address", &TelinkLightPython::set_address, bp::args("address"), "Sets the MAC address to connect to.")
    	.def("set_name", &TelinkLightPython::set_name, bp::args("name"), "Sets the device name to be used for connecting.")
      .def("set_password", &TelinkLightPython::set_password, bp::args("password"), "Sets the password to be used for connecting.")
      .def("set_vendor", &TelinkLightPython::set_vendor, bp::args("vendor"), "Sets the Bluetooth vendor code (0x0211 for Telink).")
      .def("set_mesh_id", &TelinkLightPython::set_mesh_id, bp::args("mesh_id"), "Sets device mesh ID.")
      .def("send_packet", &TelinkLightPython::send_packet, bp::args("command", "data"), "Sends a command packet to the device.")
      .def("connect", &TelinkLightPython::connect, "Connects to Bluetooth device.")
      .def("disconnect", &TelinkLightPython::disconnect, "Disconnects from Bluetooth device.")
      .def("is_connected", &TelinkLightPython::is_connected, "Probes whether the connection with the device is established.");
    
  }
}
