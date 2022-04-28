#ifndef _SIMPLE_PYTHON3_FILTER_H
#define _SIMPLE_PYTHON3_FILTER_H
/*
 * FogLAMP "Simple Python 3.x" filter class.
 *
 * Copyright (c) 2019 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Massimiliano Pinto
 */

#include <mutex>

#include <filter_plugin.h>
#include <filter.h>

#include <Python.h>

/**
 * SimplePythonFilter class is derived from FogLampFilter
 * It handles loading of a python module (provided script name)
 * and the Python3.x C API calls
 */
class SimplePythonFilter : public FogLAMPFilter
{
	public:
		SimplePythonFilter(const std::string& name,
				   ConfigCategory& config,
				   OUTPUT_HANDLE* outHandle,
				   OUTPUT_STREAM output) :
				   FogLAMPFilter(name,
						 config,
						 outHandle,
						 output)
		{};

		void	setEnableFilter(bool enable) { m_enabled = enable; };
		bool	configure();
		bool	reconfigure(const std::string& newConfig);
		void	lock() { m_configMutex.lock(); };
		void	unlock() { m_configMutex.unlock(); };
		void	logErrorMessage();

	public:
		// Python  code to execute
		std::string	m_code;

	private:
		// Configuration lock
		std::mutex	m_configMutex;
};
#endif
