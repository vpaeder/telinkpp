/** \file telink_python.h
 *  Python wrapper for C++ classes.
 *  Author: Vincent Paeder
 *  License: GPL v3
 */
#ifndef __TELINK_PYTHON_H__
#define __TELINK_PYTHON_H__

#include <boost/python.hpp>
#include <vector>
#include "../telink_light.h"

namespace bp = boost::python;

namespace telink {
  
  /** \class TelinkLightPython
   *  \brief Translator class for TelinkLight.
   */
  class TelinkLightPython : public TelinkLight {
  public:
    /** \fn TelinkLightPython(const std::string address, const std::string name, const std::string password)
     *  \brief Object instantiation.
     *  \param address : device MAC address.
     *  \param name : device name.
     *  \param password : device password.
     */
    TelinkLightPython(const std::string address, const std::string name, const std::string password) : TelinkLight(address, name, password) {}
    
    /** \fn void set_alarm(unsigned char alarm_id, bp::list & list_weekdays, unsigned char hour, unsigned char minute, unsigned char second, unsigned char action)
     *  \brief Sets an alarm with given parameters.
     *  \param alarm_id : index of the alarm
     *  \param list_weekdays : a list of 7 booleans representing which day the alarm is set on; starting index is Sunday
     *  \param hour : hour of the alarm
     *  \param minute : minute of the alarm
     *  \param second : second of the alarm
     *  \param action : what is done when the alarm triggers; 0 = switch off, 1 = switch on, >1 = start scenario with corresponding number
     */
    void set_alarm(unsigned char alarm_id, bp::list & list_weekdays, unsigned char hour, unsigned char minute, unsigned char second, unsigned char action);
    
    /** \fn void set_alarm(unsigned char alarm_id, bool state)
     *  \brief Changes the state of an alarm.
     *  \param alarm_id : index of the alarm
     *  \param state : true to set the alarm on, false to set it off
     */
    void set_alarm(unsigned char alarm_id, bool state);
    
  };
  
  /** \class TelinkLightPythonCallback
   *  \brief Python callback class for TelinkLight.
   */
  class TelinkLightPythonCallback : public TelinkLightPython {
  private:
    /** \property PyObject * self
     *  \brief Pointer to a Python TelinkLight class instance.
     */
    PyObject * self;
  
  public:
    /** \fn TelinkLightPythonCallback(PyObject *self_, const std::string address, const std::string name, const std::string password)
     *  \brief Object instantiation.
     *  \param self_ : pointer to Python TelinkLight class instance.
     *  \param address : device MAC address.
     *  \param name : device name.
     *  \param password : device password.
     */
    TelinkLightPythonCallback(PyObject *self_, const std::string address, const std::string name, const std::string password) : self(self_), TelinkLightPython(address, name, password) {
      if (!PyEval_ThreadsInitialized())
        PyEval_InitThreads();
    }
    
    ~TelinkLightPythonCallback() {}
    
    /** \fn virtual void parse_online_status_report(const std::string & packet)
     *  \brief Parses a command packet from an online status report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_online_status_report(const std::string & packet);
    
    /** \fn virtual void parse_status_report(const std::string & packet)
     *  \brief Parses a command packet from a device status report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_status_report(const std::string & packet);
    
    /** \fn virtual void parse_alarm_report(const std::string & packet)
     *  \brief Parses a command packet from an alarm report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_alarm_report(const std::string & packet);
    
    /** \fn virtual void parse_scenario_report(const std::string & packet)
     *  \brief Parses a command packet from a scenario report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_scenario_report(const std::string & packet);
    
    /** \fn virtual void parse_time_report(const std::string & packet)
     *  \brief Parses a command packet from a time report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_time_report(const std::string & packet);
    
    /** \fn virtual void parse_address_report(const std::string & packet)
     *  \brief Parses a command packet from an address report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_address_report(const std::string & packet);
    
    /** \fn virtual void parse_device_info_report(const std::string & packet)
     *  \brief Parses a command packet from a device info report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_device_info_report(const std::string & packet);
    
    /** \fn virtual void parse_group_id_report(const std::string & packet)
     *  \brief Parses a command packet from a group ID report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_group_id_report(const std::string & packet);
    
  };
  
}

#endif // __TELINK_PYTHON_H__
