#include "system/Logger.h"

#include "system/Platform.h"

#include <ctime>

namespace Sketch3D {

Logger Logger::instance_;

Logger::Logger() {
	file_.open("Log.html");
	file_ << "<html><head><style type=\"text/css\">p {margin:3}</style>" <<
			 "</head><body bgcolor=\"#8a8a8a\">" << endl;

	// This is just used to query the features of the CPU in advance
	PlatformInformation::HasCpuFeature(PlatformInformation::SSE2);
}

Logger::~Logger() {
	file_ << "</body></html>";
	file_.close();
}

Logger* Logger::getInstance() {
	return &instance_;
}

void Logger::debug(const string& message) {
	file_ << "<p style=\"background-color:#aabbcc\">[DEBUG] ";
	file_ << getCurrentTime() << " - ";
	file_ << message;
	file_ << "</p>" << endl;
}

void Logger::info(const string& message) {
	file_ << "<p>" << getCurrentTime() << " - " << message << "</p>" << endl;
}

void Logger::warning(const string& message) {
	file_ << "<p style=\"background-color:#ffbb12\">[WARNING] ";
	file_ << getCurrentTime() << " - ";
	file_ << message;
	file_ << "</p>" << endl;
}

void Logger::error(const string& message) {
	file_ << "<p style=\"background-color:#ff0012\">[ERROR] ";
	file_ << getCurrentTime() << " - ";
	file_ << message;
	file_ << "</p>" << endl;
}

string Logger::getCurrentTime() const {
	time_t t = time(0);
	struct tm* now = localtime(&t);
	string currentTime = "";
	char hour[3];
	char mins[3];
	char secs[3];

	sprintf(hour, "%02d", now->tm_hour);
	sprintf(mins, "%02d", now->tm_min);
	sprintf(secs, "%02d", now->tm_sec);

	currentTime = string(hour) + ":" + string(mins) + ":" + string(secs);
	return currentTime;
}

}