/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef UNRELIABLE_RAFT_SERVICE_H
#define UNRELIABLE_RAFT_SERVICE_H

#include "kvstore/raftex/RaftexService.h"
#include "interface/gen-cpp2/RaftexService.h"

namespace nebula {
namespace raftex {
namespace test {

class UnreliableRaftexService : public RaftexService {
public:
  static std::shared_ptr<UnreliableRaftexService> createService(
    std::shared_ptr<folly::IOThreadPoolExecutor> pool,
    std::shared_ptr<folly::Executor> workers,
    uint16_t port = 0);
  void askForVote(cpp2::AskForVoteResponse& resp, const cpp2::AskForVoteRequest& req) override;

private:
  UnreliableRaftexService() = default;
//   void appendLog(cpp2::AppendLogResponse& resp, const cpp2::AppendLogRequest& req) override;

//   void sendSnapshot(cpp2::SendSnapshotResponse& resp,
//                     const cpp2::SendSnapshotRequest& req) override;

//   void async_eb_heartbeat(
//       std::unique_ptr<apache::thrift::HandlerCallback<cpp2::HeartbeatResponse>> callback,
//       const cpp2::HeartbeatRequest& req) override;
};

}  // namespace test
}  // namespace raftex
}  // namespace nebula


#endif  // UNRELIABLE_RAFT_SERVICE_H
