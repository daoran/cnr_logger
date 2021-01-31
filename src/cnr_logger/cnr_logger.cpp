/*
 *  Software License Agreement (New BSD License)
 *
 *  Copyright 2020 National Council of Research of Italy (CNR)
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the copyright holder(s) nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <string>
#include <vector>
#include <iostream>
#include <cnr_logger/cnr_logger.h>




using log4cxx::ColorPatternLayout;
IMPLEMENT_LOG4CXX_OBJECT(ColorPatternLayout);

namespace cnr_logger
{

TraceLogger::TraceLogger()
  : logger_id_(""), filename_(""), default_values_(false), initialized_(false)
{
}


TraceLogger::TraceLogger(const std::string& logger_id, const std::string& filename,
                         const bool star_header, const bool default_values)
  : TraceLogger()
{
  try
  {
    if(!init(logger_id, filename, star_header, default_values))
    {
      std::cerr << "Error in creating the TraceLogger. " << std::endl;
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Error in creating the TraceLogger. Exception: " << e.what() << std::endl;
  }
  catch (...)
  {
    std::cerr << "Error in creating the TraceLogger. Unhandled Exception" << std::endl;
  }
}


bool TraceLogger::check(const std::string& filename)
{
  YAML::Node config = YAML::LoadFile(filename);

  bool res = true;
  res &= (config["appenders"] && config["appenders"].IsSequence());
  res &= (config["levels"]?config["levels"].IsSequence():true);
  res &= (config["pattern_layout"]?config["pattern_layout"].IsScalar():true);
  res &= (config["file_name"]?config["file_name"].IsScalar():true);
  res &= (config["append_date_to_file_name"]?config["append_date_to_file_name"].IsScalar():true);
  res &= (config["append_to_file"]?config["append_to_file"].IsScalar():true);
  
  return res;
}

bool TraceLogger::init(const std::string& logger_id, const std::string& filename,
                          const bool star_header, const bool default_values)
{
    if(initialized_)
    {
      std::string err_msg = "Logger already initialized.";
      std::cerr << err_msg << std::endl;
      return false;
    }

    logger_id_ = logger_id;
    filename_ = filename;
    default_values_ = default_values;

    if((!default_values) && (!check(filename_)))
    {
      return false;
    }

    // =======================================================================================
    // Extract the info to constructs the logger
    //
    // =======================================================================================
    YAML::Node config = YAML::LoadFile(filename);
    std::vector<std::string> appenders, levels;

    for (YAML::const_iterator it=config["appenders"].begin();it!=config["appenders"].end();++it) {
      appenders.push_back(it->as<std::string>());  
    }

    if(config["levels"])
    {
      for (YAML::const_iterator it=config["levels"].begin();it!=config["levels"].end();++it) {
        levels.push_back(it->as<std::string>());  
      }
    }
    else
    {
      std::cout << "Parameter levels does not exist" << std::endl;
    }

    if(appenders.size() != levels.size())
    {
      std::cout << "Size of appenders and levels mismatch! Default INFO level for all the appenders" << std::endl;
      levels.clear();
      levels.resize(appenders.size(), "DEBUG");
    }


    for(size_t i = 0; i < appenders.size(); i++)
    {
      std::for_each(appenders[i].begin(), appenders[i].end(), [](char & c)
      {
        c = ::tolower(c);
      });
      std::for_each(levels   [i].begin(), levels   [i].end(), [](char & c)
      {
        c = ::toupper(c);
      });
      if(appenders[i] == "file")
      {
        // nothing to do
      }
      else if((appenders[i] == "console") || (appenders[i] == "cout")
              || (appenders[i] == "terminal") || (appenders[i] == "screen") || (appenders[i] == "video"))
      {
        appenders[i] = "screen";
      }
    }

    auto it_file     = std::find(appenders.begin(), appenders.end(), "file");
    auto it_screen   = std::find(appenders.begin(), appenders.end(), "screen");
    int  idx_file    = (it_file   != appenders.end()) ? std::distance(appenders.begin(), it_file) : -1;
    int  idx_screen  = (it_screen != appenders.end()) ? std::distance(appenders.begin(), it_screen) : -1;


    if(((idx_file >= 0) && (idx_screen >= 0)) && (levels[idx_file] == levels[idx_screen]))   // 1 logger and 2 appenders
    {
      loggers_  [ SYNC_FILE_AND_CONSOLE ] = log4cxx::Logger::getLogger(logger_id_);
      levels_   [ SYNC_FILE_AND_CONSOLE ] = levels[idx_file];
    }
    else
    {
      if(idx_file >= 0)
      {
        loggers_[FILE_STREAM] = log4cxx::Logger::getLogger(logger_id_ + "_f");
        levels_ [FILE_STREAM] = levels[idx_file];
      }
      if(idx_screen >= 0)
      {
        loggers_[CONSOLE_STREAM] = log4cxx::Logger::getLogger(logger_id_);
        levels_ [CONSOLE_STREAM] = levels[idx_screen];
      }
    }


    for(auto const & level : levels_)
    {
      if(level.second == "INFO")
      {
        loggers_[level.first]->setLevel(log4cxx::Level::getInfo());
      }
      else if(level.second == "WARN")
      {
        loggers_[level.first]->setLevel(log4cxx::Level::getWarn());
      }
      else if(level.second == "ERROR")
      {
        loggers_[level.first]->setLevel(log4cxx::Level::getError());
      }
      else if(level.second == "FATAL")
      {
        loggers_[level.first]->setLevel(log4cxx::Level::getFatal());
      }
      else if(level.second == "TRACE")
      {
        loggers_[level.first]->setLevel(log4cxx::Level::getTrace());
      }
      else
      {
        loggers_[level.first]->setLevel(log4cxx::Level::getDebug());
      }
    }
    // =======================================================================================
    // =======================================================================================
    // Layout
    //
    // =======================================================================================
    std::string pattern_layout;
    if(!config["pattern_layout"])
    {
      std::cout << "Parameter pattern_layout does not exist" << std::endl;
      pattern_layout = "[%5p][%d{HH:mm:ss,SSS}][%M:%04L][%24c] %m%n";                    // add %F for file name
    }
    else
    {
      pattern_layout = config["pattern_layout"].as<std::string>();
    }
    log4cxx::ColorPatternLayoutPtr layout = new log4cxx::ColorPatternLayout(pattern_layout);
    // =======================================================================================


    // =======================================================================================
    // Configure appenders and loggers
    //
    // =======================================================================================
    std::string log_file_name;
    if(logFile() || logSyncFileAndScreen())
    {
      struct passwd *pw = getpwuid(getuid());
      const char *homedir = pw->pw_dir;
      std::string homedir_string = std::string(homedir) + "/cnr_logger";
      DIR* dir = opendir(homedir_string.c_str());
      if (dir)
      {
        closedir(dir);
      }
      else if (ENOENT == errno)
      {
        mkdir(homedir_string.c_str(),0777);
      }

      if(!config["file_name"])
      {
        log_file_name = homedir_string + "/" + logger_id_;
      }
      else
      {
        log_file_name = homedir_string + "/" + config["file_name"].as<std::string>();
      }
      bool append_date_to_file_name = false;


      if(config["append_date_to_file_name"])
      {
        
        append_date_to_file_name = config["append_date_to_file_name"].as<bool>();
      }
      time_t now = time(0);
      char* date_time = ctime(&now);

      log_file_name +=  append_date_to_file_name  ? ("." + std::string(date_time) + ".log") : ".log";

      bool append_to_file = false;
      if(config["append_to_file"])
      {
        append_to_file = config["append_date_to_file_name"].as<bool>();
      }

      log4cxx::RollingFileAppenderPtr appender = new log4cxx::RollingFileAppender(layout, log_file_name, append_to_file);
      loggers_[ logFile() ? FILE_STREAM : SYNC_FILE_AND_CONSOLE ]->addAppender(appender);

      appender->setMaximumFileSize(100 * 1024 * 1024);
      appender->setMaxBackupIndex(10);
      log4cxx::helpers::Pool pool;
      appender->activateOptions(pool);
    }


    if(logScreen() || logSyncFileAndScreen())
    {
      log4cxx::ConsoleAppenderPtr appender = new log4cxx::ConsoleAppender(layout, logger_id_);
      loggers_[  logScreen() ? CONSOLE_STREAM : SYNC_FILE_AND_CONSOLE ]->addAppender(appender);
    }

    std::string loggers_str = "n. app.: " + std::to_string(static_cast<int>(loggers_.size())) + " ";
    for(auto const & l : loggers_)
    {
      loggers_str += std::to_string(l.first) + "|";
    }

    loggers_str += (levels_.size() > 0 ?  " l: " : "");
    for(auto const & l : levels_)
    {
      loggers_str += std::to_string(l.first) + "|";
    }
    time_t now = time(0);
    char* date_time = ctime(&now);


    std::string log_start = "LOG START: " + logger_id_  + ", " + std::string(date_time)  + ", " + loggers_str
                            + (log_file_name.size() > 0 ?  ", fn: " + log_file_name : "");
    if(star_header)
    {
      CNR_INFO_ONLY_FILE((*this), "\n ==================================\n\n"
                        << "== " << log_start << "\n\n == Ready to Log!");
    }
    else
    {
      CNR_INFO_ONLY_FILE((*this), log_start);
    }

    // =======================================================================================
    // Default Throttle Time
    //
    // =======================================================================================
    if(!config["default_throttle_time"])
    {
      std::cout << "Parameter default_throttle_time does not exist" << std::endl;
      default_throttle_time_ = -1.0;
    }
    else
    {
      default_throttle_time_ = config["default_throttle_time"].as<double>();
    }

    initialized_ = true;
    return initialized_;

}


TraceLogger& TraceLogger::operator=(const TraceLogger& rhs)
{
  if(!this->init(rhs.logger_id_, rhs.filename_, false, rhs.default_values_))
  {
    std::cerr << "ERROR - THE LOGGER INITIALIZATION FAILED." << std::endl;
  }
  return *this;
}

TraceLogger::~TraceLogger()
{
  for(auto & l : loggers_)
  {
    l.second->removeAllAppenders();
  }
}

}  // namespace cnr_logger
