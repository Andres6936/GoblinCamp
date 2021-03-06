/* Copyright 2010-2011 Ilkka Halila
This file is part of Goblin Camp.

Goblin Camp is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Goblin Camp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with Goblin Camp. If not, see <http://www.gnu.org/licenses/>.*/


#include <fstream>
#include <cassert>
#include <vector>
#include <string>
#include <list>

#define BOOST_FILESYSTEM_VERSION 3

#include <boost/foreach.hpp>
#include <filesystem>
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>

namespace py = boost::python;
namespace fs = std::filesystem;

#include "Goblin/Config/Paths.hpp"
#include "Goblin/Scripting/Engine.hpp"
#include "Goblin/Scripting/API.hpp"
#include "Goblin/Scripting/API/LoggerStream.hpp"
#include "Goblin/Log/Logger.hpp"

namespace Globals
{
	py::object loadPackageFunc, printExcFunc;
	Script::API::LoggerStream stream;
}

namespace
{
	void LogBootstrapException()
	{
		py::object excType, excValue, excTB;
		Script::ExtractException(excType, excValue, excTB);
		
		try {
			py::str strExcType  = py::str(excType);
			py::str strExcValue = py::str(excValue);
			
			LOG_FUNC("Python bootstrap error: [" << py::extract<char*>(strExcType) << "] " << py::extract<char*>(strExcValue), "LogBootstrapException");
		} catch (const py::error_already_set&) {
			LOG_FUNC("< INTERNAL ERROR >", "LogBootstrapException");
			PyErr_Print();
		}
	}
}

PyMODINIT_FUNC initzlib();
PyMODINIT_FUNC initcStringIO();
PyMODINIT_FUNC init_functools();
PyMODINIT_FUNC init_weakref();
PyMODINIT_FUNC inittime();

namespace Script {
	const short version = 0;
	
	void Init(std::vector<std::string>& args) {
		LOG("Initialising the engine.");
		
		Py_NoSiteFlag = 1;
		Py_InitializeEx(0);
		Py_SetProgramName(const_cast<char*>(args[0].c_str()));
		
		init_weakref();
		inittime();
		init_functools();
		initcStringIO();
		initzlib();
		
		LOG("Python " << Py_GetVersion());
		
		// Don't use default search path.
		{
		#ifdef WINDOWS
			char pathsep = ';';
		#else
			char pathsep = ':';
		#endif
			fs::path libDir = (Paths::Get(Paths::GlobalData) / "lib");
			
			std::string path = libDir.string();
			path += pathsep;
			path += Py_GetPath(); // When need common Python modules, use ones installed in the system
			
			LOG("Python Library Path = " << path);
			PySys_SetPath(const_cast<char*>(path.c_str()));
		}
		
		try {
			// Get utility functions.
			LOG("Importing Python modules");
			py::object modImp = py::import("imp");
			py::object modTB  = py::import("traceback");
			
			Globals::printExcFunc    = modTB.attr("print_exception");
			Globals::loadPackageFunc = modImp.attr("load_package");
			
			LOG("Exposing the API.");
			ExposeAPI();
			
			LOG("Creating internal namespaces.");
			PyImport_AddModule("__gcmods__");
			PyImport_AddModule("__gcuserconfig__");
			PyImport_AddModule("__gcautoexec__");
			PyImport_AddModule("__gcdevconsole__");
			
			LOG("Setting up console namespace. If you get 'No module named XXXX' error message below, " <<
				"check " << Paths::Get(Paths::GlobalData) / "lib" << " and make sure that module is really there");
			fs::path file_name = Paths::Get(Paths::GlobalData) / "lib" / "__gcdevconsole__.py";
			if (fs::exists(file_name))
			{
				modImp.attr("load_source")("__gcdevconsole__", file_name.string());
			} else
			{
				LOG("ERROR. File not found: " << file_name );
				exit(20);
			}
			
			py::exec(
				"log.info('Console ready.')", py::import("__gcdevconsole__").attr("__dict__")
			);
		} catch (const py::error_already_set&) {
			LogBootstrapException();
			
			LOG("Bootstrap has failed, exiting.");
			exit(20);
		}
	}
	
	void Shutdown() {
		LOG("Shutting down engine.");
		
		ReleaseListeners();
		Py_Finalize();
	}
	
	void LoadScript(const std::string& mod, const std::string& directory) {
		LOG("Loading '" << directory << "' into '__gcmods__." << mod << "'.");
		
		try {
			Globals::loadPackageFunc("__gcmods__." + mod, directory);
		} catch (const py::error_already_set&) {
			LogException();
		}
	}
	
	void ExtractException(py::object& excType, py::object& excValue, py::object& excTB) {
		PyObject *rawExcType, *rawExcValue, *rawExcTB;
		PyErr_Fetch(&rawExcType, &rawExcValue, &rawExcTB);
		
		excType  = py::object(py::handle<>(rawExcType));
		
		// "The value and traceback object may be NULL even when the type object is not."
		// http://docs.python.org/c-api/exceptions.html#PyErr_Fetch
		
		// So, set them to None initially.
		excValue = py::object();
		excTB    = py::object();
		
		// And convert to py::objects when they're not NULL.
		if (rawExcValue) {
			excValue = py::object(py::handle<>(rawExcValue));
		}
		
		if (rawExcTB) {
			excTB    = py::object(py::handle<>(rawExcTB));
		}
	}
	
	void LogException(bool clear) {
		if (PyErr_Occurred() == NULL) return;
		
		py::object none, excType, excVal, excTB;
		ExtractException(excType, excVal, excTB);
		PyErr_Clear();
		
		Logger::log << "**** Python exception occurred ****\n";
		try {
			Globals::printExcFunc(excType, excVal, excTB, none, boost::ref(Globals::stream));
		} catch (const py::error_already_set&) {
			Logger::log << " < INTERNAL ERROR > \n";
			PyErr_Print();
		}
		Logger::log << Logger::Suffix();
		
		if (!clear) {
			PyErr_Restore(excType.ptr(), excVal.ptr(), excTB.ptr());
		}
	}
}
