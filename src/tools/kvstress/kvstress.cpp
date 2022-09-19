/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License.
 */
#include <folly/Format.h>
#include <folly/TokenBucket.h>
#include <folly/executors/IOThreadPoolExecutor.h>
#include <folly/init/Init.h>
#include <interface/gen-cpp2/GraphStorageService.h>
#include <sys/types.h>
#include <syscall.h>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>

#include <chrono>

DEFINE_int32(num_clients, 1, "number of clients");
DEFINE_int32(num_reqs, 10000, "number or request each client will send");
DEFINE_int32(num_io_threads, 4, "number of io threads");
DEFINE_int32(qps, 50000, "qps");
DEFINE_int32(burst_size, 25000, "burst size");
DEFINE_int32(space_id, 1, "space id");
DEFINE_int32(part_id, 1, "part id");
DEFINE_string(addr, "store1", "kv server address");
DEFINE_int32(port, 9779, "kv server port");

using nebula::cpp2::GraphSpaceID;
using nebula::cpp2::PartitionID;
using nebula::storage::cpp2::ExecResponse;
using nebula::storage::cpp2::GraphStorageServiceAsyncClient;
using nebula::storage::cpp2::KVPutRequest;

struct KvClientConfig {
  std::string id;
  int numReq;
  std::shared_ptr<folly::IOThreadPoolExecutor> ioExecutor;
  std::shared_ptr<folly::TokenBucket> rateLimiter;
  GraphSpaceID spaceId;
  PartitionID partId;
  std::string addr;
  int port;
};

class KvClient {
 public:
  explicit KvClient(const KvClientConfig &cfg);
  void join();
  void loopForever();
  std::shared_ptr<GraphStorageServiceAsyncClient> makeClient();

 private:
  KvClientConfig cfg_;
  std::unique_ptr<std::thread> worker_;
};

KvClient::KvClient(const KvClientConfig &cfg) : cfg_(cfg) {
  worker_ = std::make_unique<std::thread>([self = this] { self->loopForever(); });
}

std::shared_ptr<GraphStorageServiceAsyncClient> KvClient::makeClient() {
  folly::AsyncTransport::UniquePtr socket;
  auto evb = cfg_.ioExecutor->getEventBase();
  evb->runImmediatelyOrRunInEventBaseThreadAndWait([&socket, cfg = cfg_, evb]() {
    LOG(INFO) << folly::format("{} run within thread: {}", cfg.id, syscall(__NR_gettid));
    LOG(INFO) << folly::format("connect to server {} at port {}", cfg.addr, cfg.port);
    auto sockAddr = folly::SocketAddress(cfg.addr.c_str(), cfg.port, true);
    LOG(INFO) << "resolved addr: " << sockAddr.getAddressStr();
    // socket = folly::AsyncTransport::UniquePtr(new folly::AsyncSocket(evb, sockAddr, 10));

    socket = folly::AsyncTransport::UniquePtr(
        new folly::AsyncSocket(evb, sockAddr.getAddressStr(), cfg.port, 10));
  });

  auto channel = apache::thrift::RocketClientChannel::newChannel(std::move(socket));
  std::shared_ptr<GraphStorageServiceAsyncClient> client(
      new GraphStorageServiceAsyncClient(std::move(channel)),
      [evb](auto *p) { evb->runImmediatelyOrRunInEventBaseThreadAndWait([p] { delete p; }); });

  return client;
}

void KvClient::join() {
  worker_->join();
}

void KvClient::loopForever() {
  auto client = makeClient();
  int64_t nowts = (int64_t)std::chrono::system_clock::now().time_since_epoch().count();

  for (int i = 0; i < cfg_.numReq; i++) {
    KVPutRequest req;
    req.space_id_ref() = 1;
    auto key = folly::sformat("hello-{}-{}", cfg_.id, nowts);
    auto value = folly::sformat("world-{}-{}", cfg_.id, nowts);

    std::vector<nebula::KeyValue> kvs;
    kvs.emplace_back(std::make_pair(key, value));
    std::unordered_map<PartitionID, std::vector<nebula::KeyValue>> parts = {
        {cfg_.partId, kvs},
    };
    req.parts_ref() = parts;
    ExecResponse resp = client->future_put(req).get();
    auto &ret = resp.get_result();
    auto &fprets = ret.get_failed_parts();
    for (auto &fp : fprets) {
      LOG(ERROR) << folly::format("error putting kv: {}", static_cast<int>(fp.get_code()));
    }
  }
}

int main(int argc, char **argv) {
  folly::init(&argc, &argv);
  auto ioExecutor = std::make_shared<folly::IOThreadPoolExecutor>(FLAGS_num_io_threads);
  auto rateLimiter = std::make_shared<folly::TokenBucket>(FLAGS_qps, FLAGS_burst_size);

  auto start = std::chrono::high_resolution_clock::now();
  std::vector<std::unique_ptr<KvClient>> clients;
  for (int i = 0; i < FLAGS_num_clients; i++) {
    auto id = folly::sformat("client-{}", i + 1);
    KvClientConfig cfg;
    cfg.id = id;
    cfg.numReq = FLAGS_num_reqs;
    cfg.ioExecutor = ioExecutor;
    cfg.rateLimiter = rateLimiter;
    cfg.spaceId = FLAGS_space_id;
    cfg.partId = FLAGS_part_id;
    cfg.addr = FLAGS_addr;
    cfg.port = FLAGS_port;

    clients.emplace_back(std::make_unique<KvClient>(cfg));
  }

  for (auto &c : clients) {
    c->join();
  }

  auto end = std::chrono::high_resolution_clock::now();
  double elapsed_time_ms = std::chrono::duration<double, std::milli>(end - start).count();
  LOG(INFO) << folly::format("done, duration: {}", elapsed_time_ms);

  return 0;
}
