// Copyright 2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <chrono>
#include <memory>
#include <sstream>
#include <unistd.h>

#include <cnr_logger/cnr_logger.h>


std::shared_ptr<cnr_logger::TraceLogger> logger;

int main(int argc, char ** argv)
{
  char buff[100];
  getcwd(buff, 100);
  std::string current_working_dir(buff);
  std::string config_file_path = current_working_dir + "/../config/cnr_logger.yaml";

  logger.reset(new cnr_logger::TraceLogger("prova", config_file_path , true));

  for (size_t i = 0; i < 10; i++)
  {
    CNR_INFO(*logger, "Ciao-log-1-info");
    CNR_DEBUG(*logger, "Ciao-log-1-debug");
    CNR_FATAL(*logger, "Ciao-log-1-fatal");
    CNR_TRACE(*logger, "Ciao-log-1-trace");

    // CNR_INFO_THROTTLE(*logger,  1.0, "Ciao-log-1-info");
    // CNR_DEBUG_THROTTLE(*logger, 1.0, "Ciao-log-1-debug");
    // CNR_FATAL_THROTTLE(*logger, 1.0, "Ciao-log-1-fatal");
    // CNR_TRACE_THROTTLE(*logger, 1.0, "Ciao-log-1-trace");

    CNR_INFO_COND(*logger, true, "Ciao-log-1-info");
    CNR_DEBUG_COND(*logger, false, "Ciao-log-1debug");
    CNR_FATAL_COND(*logger, true, "Ciao-log-1fatal");
    CNR_TRACE_COND(*logger, false, "Ciao-log-1trace");

    // CNR_INFO_COND_THROTTLE(*logger, true, 1.0, "Ciao-log-1-info");
    // CNR_DEBUG_COND_THROTTLE(*logger, false, 1.0, "Ciao-log-1-debug");
    // CNR_FATAL_COND_THROTTLE(*logger, true, 1.0, "Ciao-log-1-fatal");
    // CNR_TRACE_COND_THROTTLE(*logger, false, 1.0, "Ciao-log-1-trace");

    usleep(10000);

  }
  logger.reset();
  std::cout << cnr_logger::BOLDGREEN() << "OK" << cnr_logger::RESET() << std::endl;
  return 0;
  return 0;
}


  // // Force flush of the stdout buffer.
  // setvbuf(stdout, NULL, _IONBF, BUFSIZ);

  // rclcpp::init(argc, argv);

  // auto node = rclcpp::Node::make_shared("list_parameters_async");

  // // Declare parameters that may be set on this node
  // node->declare_parameter("foo");
  // node->declare_parameter("bar");
  // node->declare_parameter("baz");
  // node->declare_parameter("foo.first");
  // node->declare_parameter("foo.second");
  // node->declare_parameter("foobar");
  // node->declare_parameter("cacca.foo");
  

  // auto parameters_client = std::make_shared<rclcpp::AsyncParametersClient>(node);

  // while (!parameters_client->wait_for_service(1s)) {
  //   if (!rclcpp::ok()) {
  //     RCLCPP_ERROR(node->get_logger(), "Interrupted while waiting for the service. Exiting.");
  //     return 0;
  //   }
  //   RCLCPP_INFO(node->get_logger(), "service not available, waiting again...");
  // }

  // RCLCPP_INFO(node->get_logger(), "Setting parameters...");
  // // Set several differnet types of parameters.
  // auto results = parameters_client->set_parameters(
  // {
  //   rclcpp::Parameter("foo", 2),
  //   rclcpp::Parameter("bar", "hello"),
  //   rclcpp::Parameter("baz", 1.45),
  //   rclcpp::Parameter("foo.first", 8),
  //   rclcpp::Parameter("foo.second", 42),
  //   rclcpp::Parameter("foobar", true),
  //   rclcpp::Parameter("cacca.foo", true),

  // });
  // // Wait for the result.
  // rclcpp::spin_until_future_complete(node, results);

  // RCLCPP_INFO(node->get_logger(), "Listing parameters...");
  // // List the details of a few parameters up to a namespace depth of 10.
  // auto parameter_list_future = parameters_client->list_parameters({"foo", "bar","baz"}, 1);

  // if (rclcpp::spin_until_future_complete(node, parameter_list_future) !=
  //   rclcpp::FutureReturnCode::SUCCESS)
  // {
  //   RCLCPP_ERROR(node->get_logger(), "service call failed, exiting tutorial.");
  //   return -1;
  // }
  // auto parameter_list = parameter_list_future.get();

  // std::stringstream ss;
  // ss << "\nParameter names:";
  // for (auto & name : parameter_list.names) {
  //   ss << "\n " << name;
  // }
  // ss << "\nParameter prefixes:";
  // for (auto & prefix : parameter_list.prefixes) {
  //   ss << "\n " << prefix;
  // }
  // RCLCPP_INFO(node->get_logger(), "%s", ss.str().c_str());

  // rclcpp::shutdown();
