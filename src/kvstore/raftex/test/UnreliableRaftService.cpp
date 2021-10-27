/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "kvstore/raftex/test/UnreliableRaftService.h"

namespace nebula {
namespace raftex {
namespace test {

std::shared_ptr<UnreliableRaftexService> UnreliableRaftexService::createService(
    std::shared_ptr<folly::IOThreadPoolExecutor> pool,
    std::shared_ptr<folly::Executor> workers,
    uint16_t port) {
  auto svc = std::shared_ptr<UnreliableRaftexService>(new UnreliableRaftexService());
//   auto svc = std::make_shared<UnreliableRaftexService>();
  CHECK(svc != nullptr) << "Failed to create a raft service";

  svc->server_ = std::make_unique<apache::thrift::ThriftServer>();
  CHECK(svc->server_ != nullptr) << "Failed to create a thrift server";
  svc->server_->setInterface(svc);

  svc->initThriftServer(pool, workers, port);
  return svc;
}

void UnreliableRaftexService::askForVote(
    cpp2::AskForVoteResponse& resp, const cpp2::AskForVoteRequest& req) {
  LOG(INFO) << "calling from hijacked raft service";
  RaftexService::askForVote(resp, req);
}

}  // namespace test
}  // namespace raftex
}  // namespace nebula
