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

#include <iostream>
#if !defined (ROS_NOT_AVAILABLE)
  #include <ros/ros.h>
#endif

#include <cnr_logger/cnr_logger.h>
#include <gtest/gtest.h>

std::string path_to_src = "";

std::shared_ptr<cnr_logger::TraceLogger> logger;
std::shared_ptr<cnr_logger::TraceLogger> logger2;


namespace detail
{
  struct unwrapper
  {
    explicit unwrapper(std::exception_ptr pe) : pe_(pe) {}

    operator bool() const
    {
      return bool(pe_);
    }

    friend auto operator<<(std::ostream& os, unwrapper const& u) -> std::ostream&
    {
      try
      {
          std::rethrow_exception(u.pe_);
          return os << "no exception";
      }
      catch(std::runtime_error const& e)
      {
          return os << "runtime_error: " << e.what();
      }
      catch(std::logic_error const& e)
      {
          return os << "logic_error: " << e.what();
      }
      catch(std::exception const& e)
      {
          return os << "exception: " << e.what();
      }
      catch(...)
      {
          return os << "non-standard exception";
      }
    }
    std::exception_ptr pe_;
  };
}

auto unwrap(std::exception_ptr pe)
{
  return detail::unwrapper(pe);
}

template<class F>
::testing::AssertionResult does_not_throw(F&& f)
{
  try
  {
     f();
     return ::testing::AssertionSuccess();
  }
  catch(...)
  {
     return ::testing::AssertionFailure() << unwrap(std::current_exception());
  }
};



TEST(TestSuite, colorTest)
{
  std::cout << "Color Test" << std::endl;
  std::cout << cnr_logger::RST() << "RESET" << " ";
  std::cout << cnr_logger::BLK() << "BLACK" << "|";
  std::cout << cnr_logger::R() << "RED" << "|";
  std::cout << cnr_logger::G() << "GREEN" << "|";
  std::cout << cnr_logger::Y() << "YELLOW" << "|";
  std::cout << cnr_logger::BLE() << "BLUE" << "|";
  std::cout << cnr_logger::M() << "MAGENTA" << "|";
  std::cout << cnr_logger::C() << "CYAN" << "|";
  std::cout << cnr_logger::W() << "WHITE" << "|";
  std::cout << cnr_logger::BBLK() << "BOLDBLACK" << "|";
  std::cout << cnr_logger::BR() << "BOLDRED" << "|";
  std::cout << cnr_logger::BG() << "BOLDGREEN" << "|";
  std::cout << cnr_logger::BY() << "BOLDYELLOW" << "|";
  std::cout << cnr_logger::BBLE() << "BOLDBLUE" << "|";
  std::cout << cnr_logger::BM() << "BOLDMAGENTA" << "|";
  std::cout << cnr_logger::BC() << "BOLDCYAN" << "|";
  std::cout << cnr_logger::BW() << "BOLDWHITE" << "|";
  std::cout << cnr_logger::RST() << std::endl;
}

std::string path(const std::string& what)
{
  std::string ret;
#if defined(ROS_NOT_AVAILABLE)
  ret = path_to_src + "/" + what + ".yaml";
#else
  ret = "/" + what ;
#endif
return ret;
}


// Declare a test
TEST(TestSuite, fullConstructor1)
{
  std::string path1 = path("file_and_screen_different_appenders");

  EXPECT_NO_FATAL_FAILURE(logger.reset(new cnr_logger::TraceLogger("log1",path1 )));
  std::string what;
  EXPECT_FALSE(logger->init("log1", path1, false, false, &what));  // Already initialized


  EXPECT_TRUE(logger->logFile());
  EXPECT_TRUE(logger->logScreen());
  EXPECT_FALSE(logger->logSyncFileAndScreen());

  EXPECT_TRUE(logger->logFatal() );
  EXPECT_TRUE(logger->logError() );
  EXPECT_TRUE(logger->logWarn()  );
  EXPECT_TRUE(logger->logInfo()  );
  EXPECT_TRUE(logger->logDebug() );
  EXPECT_TRUE(logger->logTrace() );

  std::cout << *logger << std::endl;

  EXPECT_NO_FATAL_FAILURE(logger.reset());
}


// Declare a test
TEST(TestSuite, fullConstructor2)
{
  std::string path2 = path("file_and_screen_same_appender");

  EXPECT_NO_FATAL_FAILURE(logger2.reset(new cnr_logger::TraceLogger("log2", path2)));
  EXPECT_FALSE(logger2->logFile());
  EXPECT_FALSE(logger2->logScreen());
  EXPECT_TRUE(logger2->logSyncFileAndScreen());

  EXPECT_TRUE( logger2->logFatal() );
  EXPECT_TRUE( logger2->logError() );
  EXPECT_TRUE( logger2->logWarn()  );
  EXPECT_TRUE( logger2->logInfo()  );
  EXPECT_FALSE(logger2->logDebug() );
  EXPECT_FALSE(logger2->logTrace() );

  std::cout << *logger2 << std::endl;

  EXPECT_NO_FATAL_FAILURE(logger2.reset());
}
TEST(TestSuite, partialConstructor)
{
  std::string path1 = path("file_and_screen_different_appenders");
  std::string what;
  EXPECT_NO_FATAL_FAILURE(logger.reset(new cnr_logger::TraceLogger()));
  EXPECT_TRUE(logger->init("log1", path1, false, false, &what));
  if(what.length()>0)
  {
    std::cout << what << std::endl;
  }
  EXPECT_FALSE(logger->init("log1",path1, false, false, &what));  // Already initialized
  if(what.length()>0)
  {
    std::cout << what << std::endl;
  }
  EXPECT_NO_FATAL_FAILURE(logger.reset());
}

TEST(TestSuite, wrongConstructor)
{
  EXPECT_NO_FATAL_FAILURE(logger.reset(new cnr_logger::TraceLogger( )));
  std::string what;
  EXPECT_FALSE(logger->init("log1", "/this_namespace_does_not_exist", false, false, &what));
  if(what.length()>0)
  {
    std::cout << what << std::endl;
  }
  EXPECT_TRUE(logger->init_logger("log1", "/this_namespace_does_not_exist", false, true, &what));   // default vaules depiste none parameters
  if(what.length()>0)
  {
    std::cout << what << std::endl;
  }
  // exits
  EXPECT_NO_FATAL_FAILURE(logger.reset());
}

// Declare another test
TEST(TestSuite, flushFileAndScreen)
{
  std::string path1 = path("file_and_screen_same_appender");
  std::string what;
  EXPECT_NO_FATAL_FAILURE(logger.reset(new cnr_logger::TraceLogger("log1", path1, true, true, &what)));
  if(what.length()>0)
  {
    std::cout << what << std::endl;
  }

  std::cout << *logger << std::endl;

  EXPECT_FALSE(logger->logFile());
  EXPECT_FALSE(logger->logScreen());
  EXPECT_TRUE(logger->logSyncFileAndScreen());

  for (size_t i = 0u; i < 5u; i++)
  {
    CNR_INFO(*logger, "Log info");
    CNR_WARN(*logger, "Log warn");
    CNR_DEBUG(*logger, "Log debug");
    CNR_ERROR(*logger, "Log error");
    CNR_FATAL(*logger, "Log fatal");
    CNR_TRACE(*logger, "Log trace");

    CNR_INFO_THROTTLE(*logger,  1.0, "Log info");
    CNR_WARN_THROTTLE(*logger, 1.0, "Log warn");
    CNR_DEBUG_THROTTLE(*logger, 1.0, "Log debug");
    CNR_ERROR_THROTTLE(*logger, 1.0, "Log error");
    CNR_FATAL_THROTTLE(*logger, 1.0, "Log fatal");
    CNR_TRACE_THROTTLE(*logger, 1.0, "Log trace");

    CNR_INFO_COND(*logger, true, "Log info");
    CNR_WARN_COND(*logger, true, "Log warn");
    CNR_DEBUG_COND(*logger, false, "Log debug");
    CNR_ERROR_COND(*logger, true, "Log error");
    CNR_FATAL_COND(*logger, true, "Log fatal");
    CNR_TRACE_COND(*logger, false, "Log trace");

    CNR_INFO_COND_THROTTLE(*logger, true, 1.0, "THROTTLE Log info");
    CNR_WARN_COND_THROTTLE(*logger, true, 1.0, "THROTTLE Log warn");
    CNR_DEBUG_COND_THROTTLE(*logger, false, 1.0, "THROTTLE Log debug");
    CNR_ERROR_COND_THROTTLE(*logger, true, 1.0, "THROTTLE Log error");
    CNR_FATAL_COND_THROTTLE(*logger, true, 1.0, "THROTTLE Log fatal");
    CNR_TRACE_COND_THROTTLE(*logger, false, 1.0, "THROTTLE Log trace");

#if defined(ROS_NOT_AVAILABLE)
    sleep(0.1);
#else
    ros::Duration(0.1).sleep();
#endif
  }
  EXPECT_NO_FATAL_FAILURE(logger.reset());
}

// Declare another test
TEST(TestSuite, flushOnlyFile)
{
  std::string path1 = path("only_file_streamer");

  EXPECT_NO_FATAL_FAILURE(logger.reset(new cnr_logger::TraceLogger("log1", path1, true)));

  for (size_t i = 0u; i < 5u; i++)
  {
    CNR_INFO(*logger, "Log info");
    CNR_WARN(*logger, "Log warn");
    CNR_DEBUG(*logger, "Log debug");
    CNR_FATAL(*logger, "Log fatal");
    CNR_TRACE(*logger, "Log trace");

    CNR_INFO_THROTTLE(*logger,  1.0, "Log info");
    CNR_WARN_THROTTLE(*logger, 1.0, "Log warn");
    CNR_DEBUG_THROTTLE(*logger, 1.0, "Log debug");
    CNR_FATAL_THROTTLE(*logger, 1.0, "Log fatal");
    CNR_TRACE_THROTTLE(*logger, 1.0, "Log trace");

    CNR_INFO_COND(*logger, true, "Log info");
    CNR_WARN_COND(*logger, true, "Log warn");
    CNR_DEBUG_COND(*logger, false, "Log debug");
    CNR_FATAL_COND(*logger, true, "Log fatal");
    CNR_TRACE_COND(*logger, false, "Log trace");

    CNR_INFO_COND_THROTTLE(*logger, true, 1.0, "THROTTLE Log info");
    CNR_WARN_COND_THROTTLE(*logger, true, 1.0, "THROTTLE Log warn");
    CNR_DEBUG_COND_THROTTLE(*logger, false, 1.0, "THROTTLE Log debug");
    CNR_FATAL_COND_THROTTLE(*logger, true, 1.0, "THROTTLE Log fatal");
    CNR_TRACE_COND_THROTTLE(*logger, false, 1.0, "THROTTLE Log trace");

#if defined(ROS_NOT_AVAILABLE)
    sleep(0.1);
#else
    ros::Duration(0.1).sleep();
#endif
  }
  EXPECT_NO_FATAL_FAILURE(logger.reset());
}


// Declare another test
TEST(TestSuite, flushOnlsyScreen)
{
  std::string path1=path("only_screen_streamer");

  EXPECT_NO_FATAL_FAILURE(logger.reset(new cnr_logger::TraceLogger("log1", path1, true)));

  for (size_t i = 0u; i < 5u; i++)
  {
    CNR_INFO(*logger, "Log info");
    CNR_WARN(*logger, "Log warn");
    CNR_DEBUG(*logger, "Log debug");
    CNR_FATAL(*logger, "Log fatal");
    CNR_TRACE(*logger, "Log trace");

    CNR_INFO_THROTTLE(*logger,  1.0, "Log info");
    CNR_WARN_THROTTLE(*logger, 1.0, "Log warn");
    CNR_DEBUG_THROTTLE(*logger, 1.0, "Log debug");
    CNR_FATAL_THROTTLE(*logger, 1.0, "Log fatal");
    CNR_TRACE_THROTTLE(*logger, 1.0, "Log trace");

    CNR_INFO_COND(*logger, true, "Log info");
    CNR_WARN_COND(*logger, true, "Log warn");
    CNR_DEBUG_COND(*logger, false, "Log debug");
    CNR_FATAL_COND(*logger, true, "Log fatal");
    CNR_TRACE_COND(*logger, false, "Log trace");

    CNR_INFO_COND_THROTTLE(*logger, true, 1.0, "THROTTLE Log info");
    CNR_WARN_COND_THROTTLE(*logger, true, 1.0, "THROTTLE Log warn");
    CNR_DEBUG_COND_THROTTLE(*logger, false, 1.0, "THROTTLE Log debug");
    CNR_FATAL_COND_THROTTLE(*logger, true, 1.0, "THROTTLE Log fatal");
    CNR_TRACE_COND_THROTTLE(*logger, false, 1.0, "THROTTLE Log trace");

#if defined(ROS_NOT_AVAILABLE)
    sleep(0.1);
#else
    ros::Duration(0.1).sleep();
#endif
  }
  EXPECT_NO_FATAL_FAILURE(logger.reset());
}

TEST(TestSuite, testMacros)
{
  EXPECT_NO_FATAL_FAILURE( CNR_FATAL(*logger, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_ERROR(*logger, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_WARN(*logger, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_INFO(*logger, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_INFO_ONLY_FILE(*logger, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_DEBUG(*logger, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_TRACE(*logger, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_FATAL_THROTTLE(*logger, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_ERROR_THROTTLE(*logger, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_WARN_THROTTLE(*logger, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_INFO_THROTTLE(*logger, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_DEBUG_THROTTLE(*logger, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_THROTTLE(*logger, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_FATAL_COND(*logger, 1, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_FATAL_COND(*logger, 0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_ERROR_COND(*logger, 1, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_ERROR_COND(*logger, 0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_WARN_COND(*logger, 1, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_WARN_COND(*logger, 0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_INFO_COND(*logger, 1, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_INFO_COND(*logger, 0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_DEBUG_COND(*logger, 1, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_DEBUG_COND(*logger, 0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_COND(*logger, 1, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_COND(*logger, 0, "TEST"));

  EXPECT_NO_FATAL_FAILURE( CNR_FATAL_COND_THROTTLE(*logger, 1, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_FATAL_COND_THROTTLE(*logger, 0, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_ERROR_COND_THROTTLE(*logger, 1, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_ERROR_COND_THROTTLE(*logger, 0, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_WARN_COND_THROTTLE(*logger, 1,  1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_WARN_COND_THROTTLE(*logger, 0,  1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_INFO_COND_THROTTLE(*logger, 1,  1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_INFO_COND_THROTTLE(*logger, 0,  1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_DEBUG_COND_THROTTLE(*logger, 1, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_DEBUG_COND_THROTTLE(*logger, 0, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_COND_THROTTLE(*logger, 1, 1.0, "TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_COND_THROTTLE(*logger, 0, 1.0, "TEST"));

  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_START(*logger));
  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_START(*logger,"TEST"));
  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_END(*logger));
  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_END(*logger,"TEST"));

  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_START_THROTTLE(*logger, 1.0));
  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_START_THROTTLE(*logger, 1.0, "TEST"));

  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_START_THROTTLE_DEFAULT(*logger));
  EXPECT_NO_FATAL_FAILURE( CNR_TRACE_START_THROTTLE_DEFAULT(*logger, "TEST"));

  auto f0 = [](){CNR_RETURN_BOOL(*logger, 1);};  
  EXPECT_TRUE( f0() );

  auto f1 = [](){CNR_RETURN_BOOL(*logger, 0);};
  EXPECT_FALSE( f1() );

  auto f2 = [](){CNR_RETURN_BOOL(*logger, 1,"TEST");};
  EXPECT_TRUE( f2() );

  auto f3 = [](){CNR_RETURN_BOOL(*logger, 0,"TEST");};
  EXPECT_FALSE( f3() );

  auto f4 = [](){CNR_RETURN_TRUE(*logger);};
  EXPECT_TRUE( f4() );

  auto f5 = [](){CNR_RETURN_FALSE(*logger);};
  EXPECT_FALSE( f5() );

  auto f6 = [](){CNR_RETURN_TRUE(*logger,"TEST");};
  EXPECT_TRUE( f6() );

  auto f7 = [](){CNR_RETURN_FALSE(*logger,"TEST");};
  EXPECT_FALSE( f7() );

  auto f8 = [](){CNR_RETURN_FATAL(*logger);};
  EXPECT_FALSE( f8() );

  auto f9 = [](){CNR_RETURN_FATAL(*logger,"TEST");};
  EXPECT_FALSE( f9() );

  auto f10 = [](){CNR_RETURN_OK(*logger, void());};
  EXPECT_NO_FATAL_FAILURE( f10() );

  auto f11 = [](){CNR_RETURN_OK(*logger,void(), "TEST");};
  EXPECT_NO_FATAL_FAILURE( f11() );

  auto f12 = [](){CNR_RETURN_NOTOK(*logger, void());};
  EXPECT_NO_FATAL_FAILURE( f12() );

  auto f13 = [](){CNR_RETURN_NOTOK(*logger,void(), "TEST");};
  EXPECT_NO_FATAL_FAILURE( f13() );

  auto f14 = [](){CNR_EXIT_EX(*logger, 0);};
  EXPECT_FALSE(does_not_throw([&]{ f14(); }));

  auto f15 = [](){CNR_EXIT_EX(*logger, 1);};
  EXPECT_NO_FATAL_FAILURE( f15() );

  auto f16 = [](){CNR_EXIT_EX(*logger,0, "TEST");};
  EXPECT_FALSE(does_not_throw([&]{ f16(); }));

  auto f17 = [](){CNR_EXIT_EX(*logger,1, "TEST");};
  EXPECT_NO_FATAL_FAILURE( f17() );

  auto f18 = [](){CNR_RETURN_BOOL_THROTTLE(*logger, 1, 1.0);};
  EXPECT_TRUE( f18() );

  auto f19 = [](){CNR_RETURN_BOOL_THROTTLE(*logger, 0, 1.0);};
  EXPECT_FALSE( f19() );

  auto f20 = [](){CNR_RETURN_BOOL_THROTTLE(*logger, 1,1.0, "TEST");};
  EXPECT_TRUE( f20() );

  auto f21 = [](){CNR_RETURN_BOOL_THROTTLE(*logger, 0,1.0, "TEST");};
  EXPECT_FALSE( f21() );

  auto f22 = [](){CNR_RETURN_TRUE_THROTTLE(*logger,1.0);};
  EXPECT_TRUE( f22() );

  auto f23 = [](){CNR_RETURN_FALSE_THROTTLE(*logger,1.0);};
  EXPECT_FALSE( f23() );

  auto f24 = [](){CNR_RETURN_TRUE_THROTTLE(*logger,1.0, "TEST");};
  EXPECT_TRUE( f24() );

  auto f25 = [](){CNR_RETURN_FALSE_THROTTLE(*logger,1.0, "TEST");};
  EXPECT_FALSE( f25() );

  auto f26 = [](){CNR_RETURN_OK_THROTTLE(*logger, void(), 1.0);};
  EXPECT_NO_FATAL_FAILURE( f26() );

  auto f27 = [](){CNR_RETURN_OK_THROTTLE(*logger,void(), 1.0, "TEST");};
  EXPECT_NO_FATAL_FAILURE( f27() );

  auto f28 = [](){CNR_RETURN_NOTOK_THROTTLE(*logger, void(),1.0);};
  EXPECT_NO_FATAL_FAILURE( f28() );

  auto f29 = [](){CNR_RETURN_NOTOK_THROTTLE(*logger,void(), 1.0, "TEST");};
  EXPECT_NO_FATAL_FAILURE( f29() );

  auto f30 = [](){CNR_RETURN_BOOL_THROTTLE_DEFAULT(*logger, 1);};
  EXPECT_TRUE( f30() );

  auto f31 = [](){CNR_RETURN_BOOL_THROTTLE_DEFAULT(*logger, 0);};
  EXPECT_FALSE( f31() );

  auto f32 = [](){CNR_RETURN_BOOL_THROTTLE_DEFAULT(*logger, 1, "TEST");};
  EXPECT_TRUE( f32() );

  auto f33 = [](){CNR_RETURN_BOOL_THROTTLE_DEFAULT(*logger, 0, "TEST");};
  EXPECT_FALSE( f33() );

  auto f34 = [](){CNR_RETURN_TRUE_THROTTLE_DEFAULT(*logger);};
  EXPECT_TRUE( f34() );
  
  auto f35 = [](){CNR_RETURN_FALSE_THROTTLE_DEFAULT(*logger);};
  EXPECT_FALSE( f35() );

  auto f36 = [](){CNR_RETURN_TRUE_THROTTLE_DEFAULT(*logger,"TEST");};
  EXPECT_TRUE( f36() );

  auto f37 = [](){CNR_RETURN_FALSE_THROTTLE_DEFAULT(*logger,"TEST");};
  EXPECT_FALSE( f37() );

  auto f38 = [](){CNR_RETURN_OK_THROTTLE_DEFAULT(*logger, void());};
  EXPECT_NO_FATAL_FAILURE( f38() );

  auto f39 = [](){CNR_RETURN_OK_THROTTLE_DEFAULT(*logger,void(), "TEST");};
  EXPECT_NO_FATAL_FAILURE( f39() );

  auto f40 = [](){CNR_RETURN_NOTOK_THROTTLE_DEFAULT(*logger, void());};
  EXPECT_NO_FATAL_FAILURE( f40() );

  auto f41 = [](){CNR_RETURN_NOTOK_THROTTLE_DEFAULT(*logger,void(),"TEST");};
  EXPECT_NO_FATAL_FAILURE( f41() );
}

// Run all the tests that were declared with TEST()
int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
#if !defined (ROS_NOT_AVAILABLE)
  ros::init(argc, argv, "cnr_logger_tester");
  ros::NodeHandle nh;
#else
    if(argc != 2)
    {
      std::cerr << "Error in usage!\ncnr_logger_test [ PATH_TO_SRC ]" << std::endl;
      return -1; 
    }
    path_to_src = argv[1];
#endif

#if defined(FORCE_ROS_TIME_USE)
  std::cerr << "Time used: ROS WALL TIME" << std::endl;
#else
  std::cerr << "Time used: STD CTIME" << std::endl;
#endif

  return RUN_ALL_TESTS();
}
