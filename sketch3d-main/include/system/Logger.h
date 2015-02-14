#ifndef SKETCH_3D_LOGGER_H
#define SKETCH_3D_LOGGER_H

#include "system/Platform.h"

#include <fstream>
#include <sstream>
#include <string>
using namespace std;

namespace Sketch3D {

/**
 * @enum LoggerLevel_t
 * Determines what information should the logger output. When the Logger has one of those value
 * set, it will only output this level and all the one above. This means that when set to
 * LOGGER_LEVEL_ERROR, by example, it will only output error messages but if it set to
 * LOGGER_LEVEL_INFO (default), it will ouput info messages, warnings and errors.
 */
enum LoggerLevel_t {
    LOGGER_LEVEL_ERROR,
    LOGGER_LEVEL_WARNING,
    LOGGER_LEVEL_INFO,
    LOGGER_LEVEL_DEBUG
};

/**
 * @class Logger
 * This class is a singleton and is used to log information, warnings and
 * errors into a structure html file.
 */
class SKETCH_3D_API Logger {
	public:
		/**
		 * Destructor
		 */
					   ~Logger();

		static Logger*	GetInstance();

        /**
         * Set logger level.
         * @param level The new logger level to use
         */
        void            SetLoggerLevel(LoggerLevel_t level);

		/**
		 * Write a debug message in the log
		 * @param message The message to write
		 */
		void			Debug(const string& message);

		/**
		 * Write a normal message in the log
		 * @param message The message to write
		 */
		void			Info(const string& message);

		/**
		 * Write a warning message in the log
		 * @param message The message to write
		 */
		void			Warning(const string& message);

		/**
		 * Write an error message in the log
		 * @param message The message to write
		 */
		void			Error(const string& message);
		
	private:
		static Logger	instance_;	/**< Singleton's instance */

		ofstream		file_;		/**< The file to which we're writting */
        LoggerLevel_t   level_;     /**< Logger output level */

		/**
		 * Constructor
		 */
						Logger();

		/**
		 * Copy-constructor to disallow copy
		 */
						Logger(const Logger& src);

		/**
		 * Assignment operator to disallow assignment
		 */
		Logger&			operator= (const Logger& rhs);

		/**
		 * Return the current time as a string in the format H:M:S
		 */
		string			GetCurrentTime() const;
};

}

#endif