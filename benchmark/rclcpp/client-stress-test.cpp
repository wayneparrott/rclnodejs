// Copyright (c) 2018 Intel Corporation. All rights reserved.
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
#include <utility>

#include "nav_msgs/srv/get_map.hpp"
#include "rclcpp/rclcpp.hpp"
#include "./utilities.hpp"

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);

  auto totalTimes = 0;
  printf("How many times do you want to run?\n");
  scanf("%d", &totalTimes);
  printf(
      "The client will send a GetMap request(response contains a size of 10MB "
      "array) every 100ms until receiving response %d times.\n", totalTimes);

  auto start = std::chrono::high_resolution_clock::now();
  auto node = rclcpp::Node::make_shared("stress_client_rclcpp");
  auto client = node->create_client<nav_msgs::srv::GetMap>("get_map");
  auto request = std::make_shared<nav_msgs::srv::GetMap::Request>();
  auto receivedTimes = 0;

  while (rclcpp::ok()) {
    if (receivedTimes > totalTimes) {
      rclcpp::shutdown();
      auto end = std::chrono::high_resolution_clock::now();
      LogTimeConsumption(start, end);
    } else {
      auto result_future = client->async_send_request(request);
      if (rclcpp::spin_until_future_complete(node, result_future) !=
          rclcpp::executor::FutureReturnCode::SUCCESS) {
        RCLCPP_ERROR(node->get_logger(), "service call failed.")
        return 1;
      }
      auto result = result_future.get();
      receivedTimes++;
    }
  }
  return 0;
}
