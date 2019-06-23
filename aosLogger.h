#pragma once

/* #include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
namespace logging = boost::log;
using namespace logging::trivial;
namespace src = boost::log::sources;*/

#include <glog/logging.h>

//在自己的class里面定义一个日志输出通道对象
// src::severity_channel_logger<severity_level, std::string> scl("log/11/log");
/*const int INFO = GLOG_INFO, WARNING = GLOG_WARNING,
  ERROR = GLOG_ERROR, FATAL = GLOG_FATAL;*/
// 按照日志级别写入日志
#define LOGDEBUG LOG(INFO)
#define LOGINFO LOG(INFO)
#define LOGWARN LOG(WARNING)
#define LOFERROR LOG(ERROR)
#define LOGFATAL LOG(FATAL)