#ifndef REST_RPC_COMMON_H_
#define REST_RPC_COMMON_H_

#include <vector>
#include <string>
#include <cstdint>
#include <msgpack.hpp>

namespace rest_rpc {
struct PredictParams {
  int64_t tag_id;
  int32_t encode_case;
  int32_t data_type;
  std::string data_id;
  std::string data_name;
  std::vector<std::string> buffers;
  std::string version;

  MSGPACK_DEFINE(tag_id, encode_case, data_type, data_id, data_name, buffers, version);
};

namespace rpc_service {
enum class result_code : int16_t {
  OK = 0,
  FAIL = 1,
};

enum class error_code {
  OK,
  UNKNOWN,
  FAIL,
  TIMEOUT,
  CANCEL,
  BADCONNECTION,
};

static const size_t MAX_BUF_LEN = 1048576 * 10;
static const size_t HEAD_LEN = 4;
static const size_t PAGE_SIZE = 1024 * 1024;
}  // namespace rpc_service
}  // namespace rest_rpc

#endif  // REST_RPC_COMMON_H_