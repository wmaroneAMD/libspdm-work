/**
 *  Copyright Notice:
 *  Copyright 2024 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#include "spdm_unit_test.h"
#include "internal/libspdm_responder_lib.h"

#if LIBSPDM_ENABLE_CAPABILITY_EVENT_CAP

static void set_standard_state(libspdm_context_t *spdm_context)
{
    libspdm_session_info_t *session_info;
    uint32_t session_id;

    spdm_context->connection_info.version = SPDM_MESSAGE_VERSION_13 <<
                                            SPDM_VERSION_NUMBER_SHIFT_BIT;
    spdm_context->connection_info.connection_state = LIBSPDM_CONNECTION_STATE_NEGOTIATED;
    spdm_context->response_state = LIBSPDM_RESPONSE_STATE_NORMAL;

    spdm_context->connection_info.capability.flags |=
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_EVENT_CAP;
    spdm_context->connection_info.capability.flags |=
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ENCRYPT_CAP;
    spdm_context->connection_info.capability.flags |=
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MAC_CAP;
    spdm_context->connection_info.capability.flags |=
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_EX_CAP;

    spdm_context->local_context.capability.flags |=
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ENCRYPT_CAP;
    spdm_context->local_context.capability.flags |=
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MAC_CAP;
    spdm_context->local_context.capability.flags |=
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_EX_CAP;
    spdm_context->local_context.capability.flags |=
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_EVENT_CAP;

    spdm_context->connection_info.algorithm.base_hash_algo = m_libspdm_use_hash_algo;
    spdm_context->connection_info.algorithm.base_asym_algo = m_libspdm_use_asym_algo;
    spdm_context->connection_info.algorithm.dhe_named_group = m_libspdm_use_dhe_algo;
    spdm_context->connection_info.algorithm.aead_cipher_suite = m_libspdm_use_aead_algo;

    session_id = 0xFFFFFFFF;
    spdm_context->latest_session_id = session_id;
    spdm_context->last_spdm_request_session_id_valid = true;
    spdm_context->last_spdm_request_session_id = session_id;
    session_info = &spdm_context->session_info[0];
    libspdm_session_info_init(spdm_context, session_info, session_id, true);
    libspdm_secured_message_set_session_state(
        session_info->secured_message_context,
        LIBSPDM_SESSION_STATE_ESTABLISHED);
}

/**
 * Test 1: Successful response to subscribe event types that clears all events.
 * Expected Behavior: Returns LIBSPDM_STATUS_SUCCESS with the expected values.
 **/
static void libspdm_test_responder_subscribe_event_types_ack_case1(void **state)
{
    libspdm_return_t status;
    libspdm_test_context_t *spdm_test_context;
    libspdm_context_t *spdm_context;
    spdm_subscribe_event_types_request_t spdm_request;
    size_t spdm_request_size;
    uint8_t response[LIBSPDM_MAX_SPDM_MSG_SIZE];
    size_t response_size = sizeof(response);
    spdm_subscribe_event_types_ack_response_t *spdm_response;

    spdm_test_context = *state;
    spdm_context = spdm_test_context->spdm_context;
    spdm_test_context->case_id = 1;

    set_standard_state(spdm_context);

    spdm_request.header.spdm_version = SPDM_MESSAGE_VERSION_13;
    spdm_request.header.request_response_code = SPDM_SUBSCRIBE_EVENT_TYPES;
    spdm_request.header.param1 = 0;
    spdm_request.header.param2 = 0;

    spdm_request_size = sizeof(spdm_message_header_t);

    status = libspdm_get_response_subscribe_event_types_ack(spdm_context,
                                                            spdm_request_size, &spdm_request,
                                                            &response_size, response);
    spdm_response = (spdm_subscribe_event_types_ack_response_t *)response;

    assert_int_equal(response_size, sizeof(spdm_subscribe_event_types_ack_response_t));
    assert_int_equal(status, LIBSPDM_STATUS_SUCCESS);
    assert_int_equal(spdm_response->header.spdm_version, SPDM_MESSAGE_VERSION_13);
    assert_int_equal(spdm_response->header.request_response_code, SPDM_SUBSCRIBE_EVENT_TYPES_ACK);
    assert_int_equal(spdm_response->header.param1, 0);
    assert_int_equal(spdm_response->header.param2, 0);
}

/**
 * Test 2: Successful response to subscribe event types that subscribes to two events.
 * Expected Behavior: Returns LIBSPDM_STATUS_SUCCESS with the expected values.
 **/
static void libspdm_test_responder_subscribe_event_types_ack_case2(void **state)
{
    libspdm_return_t status;
    libspdm_test_context_t *spdm_test_context;
    libspdm_context_t *spdm_context;
    spdm_subscribe_event_types_request_t *spdm_request;
    uint8_t request[LIBSPDM_MAX_SPDM_MSG_SIZE];
    size_t spdm_request_size;
    uint8_t response[LIBSPDM_MAX_SPDM_MSG_SIZE];
    size_t response_size = sizeof(response);
    spdm_subscribe_event_types_ack_response_t *spdm_response;

    spdm_test_context = *state;
    spdm_context = spdm_test_context->spdm_context;
    spdm_test_context->case_id = 2;

    set_standard_state(spdm_context);

    spdm_request = (spdm_subscribe_event_types_request_t *)request;

    spdm_request->header.spdm_version = SPDM_MESSAGE_VERSION_13;
    spdm_request->header.request_response_code = SPDM_SUBSCRIBE_EVENT_TYPES;
    spdm_request->header.param1 = 1;
    spdm_request->header.param2 = 0;
    spdm_request->subscribe_list_len = 0;

    generate_dmtf_event_group(spdm_request + 1, (uint8_t *)&spdm_request->subscribe_list_len,
                              0, true, true, false, false);

    spdm_request_size = sizeof(spdm_subscribe_event_types_request_t) +
                        spdm_request->subscribe_list_len;

    status = libspdm_get_response_subscribe_event_types_ack(spdm_context,
                                                            spdm_request_size, spdm_request,
                                                            &response_size, response);
    spdm_response = (spdm_subscribe_event_types_ack_response_t *)response;

    assert_int_equal(response_size, sizeof(spdm_subscribe_event_types_ack_response_t));
    assert_int_equal(status, LIBSPDM_STATUS_SUCCESS);
    assert_int_equal(spdm_response->header.spdm_version, SPDM_MESSAGE_VERSION_13);
    assert_int_equal(spdm_response->header.request_response_code, SPDM_SUBSCRIBE_EVENT_TYPES_ACK);
    assert_int_equal(spdm_response->header.param1, 0);
    assert_int_equal(spdm_response->header.param2, 0);
}

int libspdm_responder_subscribe_event_types_ack_test_main(void)
{
    libspdm_test_context_t test_context = {
        LIBSPDM_TEST_CONTEXT_VERSION,
        false,
    };

    const struct CMUnitTest spdm_responder_subscribe_event_types_ack_tests[] = {
        cmocka_unit_test(libspdm_test_responder_subscribe_event_types_ack_case1),
        cmocka_unit_test(libspdm_test_responder_subscribe_event_types_ack_case2)
    };

    libspdm_setup_test_context(&test_context);

    return cmocka_run_group_tests(spdm_responder_subscribe_event_types_ack_tests,
                                  libspdm_unit_test_group_setup,
                                  libspdm_unit_test_group_teardown);
}

#endif /* LIBSPDM_ENABLE_CAPABILITY_EVENT_CAP */