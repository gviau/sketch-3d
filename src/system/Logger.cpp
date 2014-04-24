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

Logger* Logger::GetInstance() {
	return &instance_;
}

void Logger::Debug(const string& message) {
	file_ << "<p style=\"background-color:#aabbcc\">[DEBUG] ";
	file_ << GetCurrentTime() << " - ";
	file_ << message;
	file_ << "</p>" << endl;
}

void Logger::Info(const string& message) {
	file_ << "<p>" << GetCurrentTime() << " - " << message << "</p>" << endl;
}

void Logger::Warning(const string& message) {
	file_ << "<p style=\"background-color:#ffbb12\">[WARNING] ";
	file_ << GetCurrentTime() << " - ";
	file_ << message;
	file_ << "</p>" << endl;
}

void Logger::Error(const string& message) {
	file_ << "<p style=\"background-color:#ff0012\">[ERROR] ";
	file_ << GetCurrentTime() << " - ";
	file_ << message;
	file_ << "</p>" << endl;
}

string Logger::GetCurrentTime() const {
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