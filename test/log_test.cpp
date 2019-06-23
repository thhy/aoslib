#include <glog/logging.h>

int main(int argc, char **argv)
{
    google::InitGoogleLogging(argv[0]);
    FLAGS_log_dir = "/home/czy/git/aoslib/test/log";
    LOG(INFO) << "INFO";
    LOG(INFO) << "INFO1";
    LOG(WARNING) << "WARNING";
    LOG(WARNING) << "WARNING1";
    LOG(ERROR) << "ERROR";
    LOG(ERROR) << "ERROR1";
    LOG(FATAL) << "FATAL";
    google::ShutdownGoogleLogging();
}