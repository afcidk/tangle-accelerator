/*
 * Copyright (C) 2018-2019 BiiLabs Co., Ltd. and Contributors
 * All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the MIT license. A copy of the license can be found in the file
 * "LICENSE" at the root of this distribution.
 */

#include "proxy_apis.h"
#include "utils/handles/lock.h"
#include "utils/hash_algo_djb2.h"

#define PROXY_APIS_LOGGER "proxy_apis"

static logger_id_t logger_id;
static lock_handle_t cjson_lock;

void proxy_apis_logger_init() { logger_id = logger_helper_enable(PROXY_APIS_LOGGER, LOGGER_DEBUG, true); }

int proxy_apis_logger_release() {
  logger_helper_release(logger_id);
  if (logger_helper_destroy() != RC_OK) {
    ta_log_error("Destroying logger failed %s.\n", PROXY_APIS_LOGGER);
    return EXIT_FAILURE;
  }

  return 0;
}

status_t proxy_apis_lock_init() {
  if (lock_handle_init(&cjson_lock)) {
    return SC_CONF_LOCK_INIT;
  }
  return SC_OK;
}

status_t proxy_apis_lock_destroy() {
  if (lock_handle_destroy(&cjson_lock)) {
    return SC_CONF_LOCK_DESTROY;
  }
  return SC_OK;
}

/**
 * @brief Store response message to json_result
 * @param[in] res_buff Buffer of response message
 * @param[out] json_result Response message
 *
 * @return
 * - SC_OK on success
 * - non-zero on error
 */
static status_t char_buffer_to_str(char_buffer_t* res_buff, char** json_result) {
  if (res_buff == NULL) {
    ta_log_error("%s\n", "SC_TA_NULL");
    return SC_TA_NULL;
  }

  *json_result = (char*)malloc((res_buff->length + 1) * sizeof(char));
  if (*json_result == NULL) {
    ta_log_error("%s\n", "SC_TA_OOM");
    return SC_TA_OOM;
  }
  snprintf(*json_result, (res_buff->length + 1), "%s", res_buff->data);

  return SC_OK;
}

static status_t api_check_consistency(const iota_client_service_t* const service, const char* const obj,
                                      char** json_result) {
  status_t ret = SC_OK;
  check_consistency_req_t* req = check_consistency_req_new();
  check_consistency_res_t* res = check_consistency_res_new();
  char_buffer_t* res_buff = char_buffer_new();
  if (req == NULL || res == NULL || res_buff == NULL) {
    ret = SC_TA_OOM;
    ta_log_error("%s\n", "SC_TA_OOM");
    goto done;
  }

  lock_handle_lock(&cjson_lock);
  if (service->serializer.vtable.check_consistency_deserialize_request(obj, req) != RC_OK) {
    lock_handle_unlock(&cjson_lock);
    ret = SC_CCLIENT_JSON_PARSE;
    ta_log_error("%s\n", "SC_CCLIENT_JSON_PARSE");
    goto done;
  }
  lock_handle_unlock(&cjson_lock);

  lock_handle_lock(&cjson_lock);
  if (iota_client_check_consistency(service, req, res) != RC_OK) {
    lock_handle_unlock(&cjson_lock);
    ret = SC_CCLIENT_FAILED_RESPONSE;
    ta_log_error("%s\n", "SC_CCLIENT_FAILED_RESPONSE");
    goto done;
  }
  lock_handle_unlock(&cjson_lock);

  if (service->serializer.vtable.check_consistency_serialize_response(res, res_buff) != RC_OK) {
    ret = SC_CCLIENT_JSON_PARSE;
    ta_log_error("%s\n", "SC_CCLIENT_JSON_PARSE");
    goto done;
  }

  ret = char_buffer_to_str(res_buff, json_result);

done:
  check_consistency_req_free(&req);
  check_consistency_res_free(&res);
  char_buffer_free(res_buff);
  return ret;
}

static status_t api_find_transactions(const iota_client_service_t* const service, const char* const obj,
                                      char** json_result) {
  status_t ret = SC_OK;
  find_transactions_req_t* req = find_transactions_req_new();
  find_transactions_res_t* res = find_transactions_res_new();
  char_buffer_t* res_buff = char_buffer_new();
  if (req == NULL || res == NULL || res_buff == NULL) {
    ret = SC_TA_OOM;
    ta_log_error("%s\n", "SC_TA_OOM");
    goto done;
  }
  lock_handle_lock(&cjson_lock);
  if (service->serializer.vtable.find_transactions_deserialize_request(obj, req) != RC_OK) {
    lock_handle_unlock(&cjson_lock);
    ret = SC_CCLIENT_JSON_PARSE;
    ta_log_error("%s\n", "SC_CCLIENT_JSON_PARSE");
    goto done;
  }
  lock_handle_unlock(&cjson_lock);

  lock_handle_lock(&cjson_lock);
  if (iota_client_find_transactions(service, req, res) != RC_OK) {
    lock_handle_unlock(&cjson_lock);
    ret = SC_CCLIENT_FAILED_RESPONSE;
    ta_log_error("%s\n", "SC_CCLIENT_FAILED_RESPONSE");
    goto done;
  }
  lock_handle_unlock(&cjson_lock);

  if (service->serializer.vtable.find_transactions_serialize_response(res, res_buff) != RC_OK) {
    ret = SC_CCLIENT_JSON_PARSE;
    ta_log_error("%s\n", "SC_CCLIENT_JSON_PARSE");
    goto done;
  }

  ret = char_buffer_to_str(res_buff, json_result);

done:
  find_transactions_req_free(&req);
  find_transactions_res_free(&res);
  char_buffer_free(res_buff);
  return ret;
}

static status_t api_get_balances(const iota_client_service_t* const service, const char* const obj,
                                 char** json_result) {
  status_t ret = SC_OK;
  get_balances_req_t* req = get_balances_req_new();
  get_balances_res_t* res = get_balances_res_new();
  char_buffer_t* res_buff = char_buffer_new();
  if (req == NULL || res == NULL || res_buff == NULL) {
    ret = SC_TA_OOM;
    ta_log_error("%s\n", "SC_TA_OOM");
    goto done;
  }

  lock_handle_lock(&cjson_lock);
  if (service->serializer.vtable.get_balances_deserialize_request(obj, req) != RC_OK) {
    lock_handle_unlock(&cjson_lock);
    ret = SC_CCLIENT_JSON_PARSE;
    ta_log_error("%s\n", "SC_CCLIENT_JSON_PARSE");
    goto done;
  }
  lock_handle_unlock(&cjson_lock);

  lock_handle_lock(&cjson_lock);
  if (iota_client_get_balances(service, req, res) != RC_OK) {
    lock_handle_unlock(&cjson_lock);
    ret = SC_CCLIENT_FAILED_RESPONSE;
    ta_log_error("%s\n", "SC_CCLIENT_FAILED_RESPONSE");
    goto done;
  }
  lock_handle_unlock(&cjson_lock);

  if (service->serializer.vtable.get_balances_serialize_response(res, res_buff) != RC_OK) {
    ret = SC_CCLIENT_JSON_PARSE;
    ta_log_error("%s\n", "SC_CCLIENT_JSON_PARSE");
    goto done;
  }

  ret = char_buffer_to_str(res_buff, json_result);

done:
  get_balances_req_free(&req);
  get_balances_res_free(&res);
  char_buffer_free(res_buff);
  return ret;
}

static status_t api_get_inclusion_states(const iota_client_service_t* const service, const char* const obj,
                                         char** json_result) {
  status_t ret = SC_OK;
  get_inclusion_states_req_t* req = get_inclusion_states_req_new();
  get_inclusion_states_res_t* res = get_inclusion_states_res_new();
  char_buffer_t* res_buff = char_buffer_new();
  if (req == NULL || res == NULL || res_buff == NULL) {
    ret = SC_TA_OOM;
    ta_log_error("%s\n", "SC_TA_OOM");
    goto done;
  }

  lock_handle_lock(&cjson_lock);
  if (service->serializer.vtable.get_inclusion_states_deserialize_request(obj, req) != RC_OK) {
    lock_handle_unlock(&cjson_lock);
    ret = SC_CCLIENT_JSON_PARSE;
    ta_log_error("%s\n", "SC_CCLIENT_JSON_PARSE");
    goto done;
  }
  lock_handle_unlock(&cjson_lock);

  lock_handle_lock(&cjson_lock);
  if (iota_client_get_inclusion_states(service, req, res) != RC_OK) {
    lock_handle_unlock(&cjson_lock);
    ret = SC_CCLIENT_FAILED_RESPONSE;
    ta_log_error("%s\n", "SC_CCLIENT_FAILED_RESPONSE");
    goto done;
  }
  lock_handle_unlock(&cjson_lock);

  if (service->serializer.vtable.get_inclusion_states_serialize_response(res, res_buff) != RC_OK) {
    ret = SC_CCLIENT_JSON_PARSE;
    ta_log_error("%s\n", "SC_CCLIENT_JSON_PARSE");
    goto done;
  }

  ret = char_buffer_to_str(res_buff, json_result);

done:
  get_inclusion_states_req_free(&req);
  get_inclusion_states_res_free(&res);
  char_buffer_free(res_buff);
  return ret;
}

static status_t api_get_node_info(const iota_client_service_t* const service, char** json_result) {
  status_t ret = SC_OK;
  get_node_info_res_t* res = get_node_info_res_new();
  char_buffer_t* res_buff = char_buffer_new();
  if (res == NULL || res_buff == NULL) {
    ret = SC_TA_OOM;
    ta_log_error("%s\n", "SC_TA_OOM");
    goto done;
  }

  lock_handle_lock(&cjson_lock);
  if (iota_client_get_node_info(service, res) != RC_OK) {
    lock_handle_unlock(&cjson_lock);
    ret = SC_CCLIENT_FAILED_RESPONSE;
    ta_log_error("%s\n", "SC_CCLIENT_FAILED_RESPONSE");
    goto done;
  }
  lock_handle_unlock(&cjson_lock);

  if (service->serializer.vtable.get_node_info_serialize_response(res, res_buff) != RC_OK) {
    ret = SC_CCLIENT_JSON_PARSE;
    ta_log_error("%s\n", "SC_CCLIENT_JSON_PARSE");
    goto done;
  }

  ret = char_buffer_to_str(res_buff, json_result);

done:
  get_node_info_res_free(&res);
  char_buffer_free(res_buff);
  return ret;
}

static status_t api_get_trytes(const iota_client_service_t* const service, const char* const obj, char** json_result) {
  status_t ret = SC_OK;
  get_trytes_req_t* req = get_trytes_req_new();
  get_trytes_res_t* res = get_trytes_res_new();
  char_buffer_t* res_buff = char_buffer_new();
  if (req == NULL || res == NULL || res_buff == NULL) {
    ret = SC_TA_OOM;
    ta_log_error("%s\n", "SC_TA_OOM");
    goto done;
  }

  lock_handle_lock(&cjson_lock);
  if (service->serializer.vtable.get_trytes_deserialize_request(obj, req) != RC_OK) {
    lock_handle_unlock(&cjson_lock);
    ret = SC_CCLIENT_JSON_PARSE;
    ta_log_error("%s\n", "SC_CCLIENT_JSON_PARSE");
    goto done;
  }
  lock_handle_unlock(&cjson_lock);

  lock_handle_lock(&cjson_lock);
  if (iota_client_get_trytes(service, req, res) != RC_OK) {
    lock_handle_unlock(&cjson_lock);
    ret = SC_CCLIENT_FAILED_RESPONSE;
    ta_log_error("%s\n", "SC_CCLIENT_FAILED_RESPONSE");
    goto done;
  }
  lock_handle_unlock(&cjson_lock);

  if (service->serializer.vtable.get_trytes_serialize_response(res, res_buff) != RC_OK) {
    ret = SC_CCLIENT_JSON_PARSE;
    ta_log_error("%s\n", "SC_CCLIENT_JSON_PARSE");
    goto done;
  }

  ret = char_buffer_to_str(res_buff, json_result);

done:
  get_trytes_req_free(&req);
  get_trytes_res_free(&res);
  char_buffer_free(res_buff);
  return ret;
}

status_t api_proxy_apis(const iota_client_service_t* const service, const char* const obj, char** json_result) {
  status_t ret = SC_OK;
  const uint8_t max_cmd_len = 30;
  char command[max_cmd_len];

  proxy_apis_command_req_deserialize(obj, command);
  // TODO more proxy APIs should be implemented
  enum {
    H_checkConsistency = 3512812181U,
    H_findTransactions = 1314581663U,
    H_getBalances = 185959358U,
    H_getInclusionStates = 3627925933U,
    H_getNodeInfo = 1097192119U,
    H_getTrytes = 3016694096U
  };

  // With DJB2 hash function we could reduce the amount of string comparisons and deterministic execution time
  switch (hash_algo_djb2(command)) {
    case H_checkConsistency:
      ret = api_check_consistency(service, obj, json_result);
      break;
    case H_findTransactions:
      ret = api_find_transactions(service, obj, json_result);
      break;
    case H_getBalances:
      ret = api_get_balances(service, obj, json_result);
      break;
    case H_getInclusionStates:
      ret = api_get_inclusion_states(service, obj, json_result);
      break;
    case H_getNodeInfo:
      ret = api_get_node_info(service, json_result);
      break;
    case H_getTrytes:
      ret = api_get_trytes(service, obj, json_result);
      break;

    default:
      ta_log_error("%s\n", "SC_HTTP_URL_NOT_MATCH");
      return SC_HTTP_URL_NOT_MATCH;
  }

  if (ret) {
    ta_log_error("%d\n", ret);
  }
  return ret;
}
