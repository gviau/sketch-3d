#include "system/Logger.h"

#include "system/Platform.h"

#include <ctime>

namespace Sketch3D {

Logger Logger::instance_;

Logger::Logger() : level_(LOGGER_LEVEL_INFO) {
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

Logger* Logger::GetInstance() {
	return &instance_;
}

void Logger::SetLoggerLevel(LoggerLevel_t level) {
    level_ = level;
}

void Logger::Debug(const string& message) {
    if (level_ >= LOGGER_LEVEL_DEBUG) {
	    file_ << "<p style=\"background-color:#aabbcc\">[DEBUG] ";
	    file_ << GetCurrentTime() << " - ";
	    file_ << message;
	    file_ << "</p>" << endl;
    }
}

void Logger::Info(const string& message) {
    if (level_ >= LOGGER_LEVEL_INFO) {
	    file_ << "<p>" << GetCurrentTime() << " - " << message << "</p>" << endl;
    }
}

void Logger::Warning(const string& message) {
    if (level_ >= LOGGER_LEVEL_WARNING) {
	    file_ << "<p style=\"background-color:#ffbb12\">[WARNING] ";
	    file_ << GetCurrentTime() << " - ";
	    file_ << message;
	    file_ << "</p>" << endl;
    }
}

void Logger::Error(const string& message) {
    if (level_ >= LOGGER_LEVEL_ERROR) {
	    file_ << "<p style=\"background-color:#ff0012\">[ERROR] ";
	    file_ << GetCurrentTime() << " - ";
	    file_ << message;
	    file_ << "</p>" << endl;
    }
}

string Logger::GetCurrentTime() const {
	time_t t = time(0);
	struct tm now;
    localtime_s(&now, &t);

	string currentTime = "";
	char hour[3];
	char mins[3];
	char secs[3];

	sprintf_s(hour, 3, "%02d", now.tm_hour);
	sprintf_s(mins, 3, "%02d", now.tm_min);
	sprintf_s(secs, 3, "%02d", now.tm_sec);

	currentTime = string(hour) + ":" + string(mins) + ":" + string(secs);
	return currentTime;
}

}