#ifndef SKETCH_3D_LOGGER_H
#define SKETCH_3D_LOGGER_H

#include <fstream>
#include <sstream>
#include <string>
using namespace std;

namespace Sketch3D {

/**
 * @class Logger
 * This class is a singleton and is used to log information, warnings and
 * errors into a structure html file.
 */
class Logger {
	public:
		/**
		 * Destructor
		 */
					   ~Logger();

		static Logger*	getInstance();

		/**
		 * Write a debug message in the log
		 * @param message The message to write
		 */
		void			debug(const string& message);

		/**
		 * Write a normal message in the log
		 * @param message The message to write
		 */
		void			info(const string& message);

		/**
		 * Write a warning message in the log
		 * @param message The message to write
		 */
		void			warning(const string& message);

		/**
		 * Write an error message in the log
		 * @param message The message to write
		 */
		void			error(const string& message);
		
	private:
		static Logger	instance_;	/**< Singleton's instance */

		ofstream		file_;		/**< The file to which we're writting */

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
		string			getCurrentTime() const;
};

}

#endif