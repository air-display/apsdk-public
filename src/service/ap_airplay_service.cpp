#include <stdexcept>
#include <exception>
#include <ctime>
#include <playfair/fair_play.h>
#include <utils/logger.h>
#include <utils/plist.h>
#include <utils/utils.h>
#include "ap_types.h"
#include "ap_video_stream_service.h"
#include "ap_audio_stream_service.h"
#include "ap_airplay_service.h"

#define DUMP_REQUEST(x) x.dump()

using namespace aps::service::details;

namespace aps { namespace service { 
    ap_airplay_session::ap_airplay_session(asio::io_context& io_ctx, ap_config& config)
        : tcp_session_base(io_ctx)
        , config_(config)
    {
        register_request_handlers();
        LOGI() << "ap_airplay_session (" << std::hex << this << ") is being created";
    }

    ap_airplay_session::~ap_airplay_session()
    {
        LOGI() << "ap_airplay_session (" << std::hex << this << ") is being destroyed";
    }

    void ap_airplay_session::register_rtsp_request_handler(
        request_hanlder handler,
        const std::string& method, 
        const std::string& path /*= std::string()*/)
    {
        auto path_map = rtsp_request_handlers_.find(method);
        if (path_map == rtsp_request_handlers_.end())
        {
            path_handler_map path_map;
            path_map[path] = handler;
            rtsp_request_handlers_[method] = path_map;
        }
        else
        {
            path_map->second[path] = handler;
        }
    }

    void ap_airplay_session::register_http_request_handler(
        request_hanlder handler, 
        const std::string& method, 
        const std::string& path /*= std::string()*/)
    {
        auto path_map = http_request_handlers_.find(method);
        if (path_map == http_request_handlers_.end())
        {
            path_handler_map path_map;
            path_map[path] = handler;
            http_request_handlers_[method] = path_map;
        }
        else
        {
            path_map->second[path] = handler;
        }
    }

    void ap_airplay_session::start()
    {
        post_receive_request_head();
    }
    
    void ap_airplay_session::options_handler(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok)
            .with_header("Public", "ANNOUNCE, SETUP, RECORD, PAUSE, FLUSH, TEARDOWN, OPTIONS, GET_PARAMETER, SET_PARAMETER, POST, GET")
            .with_content_type(APPLICATION_OCTET_STREAM);
    }

    void ap_airplay_session::post_pair_setup_handler(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        if (32 == req.body.size())
        {
            crypto_.init_client_public_keys(
                0, 0, req.body.data(), 32);
        }

        res.with_status(ok)
            .with_content(crypto_.server_keys().ed_public_key())
            .with_content_type(APPLICATION_OCTET_STREAM);
    }

    void ap_airplay_session::post_pair_verify_handler(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        pair_verify_header_t *header = (pair_verify_header_t *)req.body.data();
        if (header->is_first_frame)
        {
            crypto_.init_client_public_keys(
                req.body.data() + 4, 32,        // client curve public key
                req.body.data() + 4 + 32, 32);  // client ed public key

            crypto_.init_pair_verify_aes();

            std::vector<uint8_t> signature;
            crypto_.sign_pair_signature(signature);

            std::vector<uint8_t> res_data = crypto_.server_keys().curve_public_key();
            std::copy(signature.begin(), signature.end(),
                std::back_inserter(res_data));

            res.with_status(ok)
                .with_content(res_data)
                .with_content_type(APPLICATION_OCTET_STREAM);

            return;
        }
        else
        {
            if (crypto_.verify_pair_signature(req.body.data() + 4, 64))
            {
                res.with_status(ok);
            }
            else
            {
                res.with_status(bad_request);
            }
        }
    }

    void ap_airplay_session::post_fp_setup_handler(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        if (req.body.size() == 16)
        {
            uint8_t ver = req.body[4];
            if (ver == 0x03)
            {
                uint8_t mode = req.body[14];
                std::vector<uint8_t> content;
                uint8_t* pos = (uint8_t*)&(reply_message[mode]);
                std::copy(pos, pos + 142, std::back_inserter(content));
                res.with_status(ok)
                    .with_content_type(APPLICATION_OCTET_STREAM)
                    .with_content(content);
            }
        }
        else if (req.body.size() == 164)
        {
            uint8_t ver = req.body[4];
            if (ver == 0x03)
            {
                std::vector<uint8_t> content;
                uint8_t* pos = (uint8_t*)&(fp_header);
                std::copy(pos, pos + 12, std::back_inserter(content));
                pos = (uint8_t*)&(req.body[144]);
                std::copy(pos, pos + 20, std::back_inserter(content));
                res.with_status(ok)
                    .with_content_type(APPLICATION_OCTET_STREAM)
                    .with_content(content);
            }
        }
        else
        {
            LOGE() << "Invalid request";
            res.with_status(bad_request);
            return;
        }

        res.with_status(ok)
            .with_content_type(APPLICATION_OCTET_STREAM);
    }

    void ap_airplay_session::setup_handler(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        do
        {
            if (req.content_type.compare(APPLICATION_BINARY_PLIST))
                break;

            auto_plist plist_obj = plist_object_from_bplist(req.body.data(), req.body.size());
            auto data_obj = plist_obj.get();
            if (!data_obj)
                break;

            auto streams = plist_object_dict_get_value(data_obj, "streams");
            if (streams)
            {
                if (PLIST_TYPE_ARRAY != plist_object_get_type(streams))
                    break;

                auto stream_obj = plist_object_array_get_value(streams, 0);
                if (!stream_obj || PLIST_TYPE_DICT != plist_object_get_type(stream_obj))
                    break;

                auto type_obj = plist_object_dict_get_value(stream_obj, "type");
                if (!type_obj || PLIST_TYPE_INTEGER != plist_object_get_type(type_obj))
                    break;

                int64_t type = 0;
                if (0 != plist_object_integer_get_value(type_obj, &type))
                    break;

                if (stream_type_t::audio == type)
                {
                    if (!audio_stream_service_)
                    {
                        audio_stream_service_ = std::make_shared<ap_audio_stream_service>(crypto_);
                        //audio_stream_service_->start();
                    }

                    auto_plist audio_stream = plist_object_dict(1,
                        "streams", plist_object_array(1,
                            plist_object_dict(3,
                                "type", plist_object_integer(type),
                                "dataPort", plist_object_integer(audio_stream_service_->port()),
                                "controlPort", plist_object_integer(audio_stream_service_->port())
                            )
                        )
                    );

                    res.with_status(ok)
                        .with_content(audio_stream.to_bytes_array())
                        .with_content_type(APPLICATION_BINARY_PLIST);

                    return;
                }
                else if (stream_type_t::video == type)
                {
                    auto connection_id_obj = plist_object_dict_get_value(stream_obj, "streamConnectionID");
                    if (PLIST_TYPE_INTEGER != plist_object_get_type(connection_id_obj))
                        break;

                    int64_t connection_id = 0;
                    if (0 != plist_object_integer_get_value(connection_id_obj, &connection_id))
                        break;

                    crypto_.init_video_stream_aes(connection_id);

                    if (!video_stream_service_)
                    {
                        video_stream_service_ = std::make_shared<ap_video_stream_service>(crypto_);
                        video_stream_service_->start();
                    }

                    auto_plist video_stream = plist_object_dict(1,
                        "streams", plist_object_array(1,
                            plist_object_dict(2,
                                "type", plist_object_integer(type),
                                "dataPort", plist_object_integer(video_stream_service_->port())
                            )
                        )
                    );

                    res.with_status(ok)
                        .with_content(video_stream.to_bytes_array())
                        .with_content_type(APPLICATION_BINARY_PLIST);

                    return;
                }
                else
                    break;
            }
            else
            {
                const uint8_t* piv = 0;
                uint64_t iv_len = 0;
                auto eiv_obj = plist_object_dict_get_value(data_obj, "eiv");
                if (0 != plist_object_data_get_value(eiv_obj, &piv, &iv_len))
                    break;

                const uint8_t* pkey = 0;
                uint64_t key_len = 0;
                auto ekey_obj = plist_object_dict_get_value(data_obj, "ekey");
                if (0 != plist_object_data_get_value(ekey_obj, &pkey, &key_len))
                    break;

                crypto_.init_client_rsa_info(piv, iv_len, pkey, key_len);

                auto timing_port_obj = plist_object_dict_get_value(data_obj, "timingPort");
                if (PLIST_TYPE_INTEGER != plist_object_get_type(timing_port_obj))
                    break;
                int64_t timing_port = 0;
                if (0 != plist_object_integer_get_value(timing_port_obj, &timing_port))
                    break;

                auto_plist content = plist_object_dict(2,
                    "eventPort", plist_object_integer(0),
                    "timingPort", plist_object_integer(timing_port));

                res.with_status(ok)
                    .with_content_type(APPLICATION_BINARY_PLIST)
                    .with_content(content.to_bytes_array());

                return;
            }
        } while (0);

        // Bad request
        res.with_status(bad_request);
        return;
    }

    void ap_airplay_session::get_info_handler(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        auto_plist info = plist_object_dict(15,
            "deviceID", plist_object_string(config_.deviceID().c_str()),
            "features", plist_object_integer(config_.features()),
            "keepAliveLowPower", plist_object_integer(1),
            "keepAliveSendStatsAsBody", plist_object_integer(1),
            "macAddress", plist_object_string(config_.macAddress().c_str()),
            "model", plist_object_string(config_.model().c_str()),
            "name", plist_object_string(config_.name().c_str()),
            "sourceVersion", plist_object_string(config_.sourceVersion().c_str()),
            "statusFlags", plist_object_integer(config_.statusFlag()),
            "pi", plist_object_string(config_.pi().c_str()),
            "pk", plist_object_data((uint8_t *)config_.pk().c_str(), config_.pk().length()),
            "vv", plist_object_integer(config_.vv()),
            "audioFormats", plist_object_array(1,
                plist_object_dict(3,
                    "type", plist_object_integer(96),
                    "audioInputFormats", plist_object_integer(0x01000000),
                    "audioOutputFormats", plist_object_integer(0x01000000)
                )
            ),
            "audioLatencies", plist_object_array(1,
                plist_object_dict(4,
                    "type", plist_object_integer(96),
                    "audioType", plist_object_string("default"),
                    "inputLatencyMicros", plist_object_integer(3),
                    "outputLatencyMicros", plist_object_integer(79)
                )
            ),
            "displays", plist_object_array(1,
                plist_object_dict(11,
                    "features", plist_object_integer(14),
                    "height", plist_object_integer(config_.display().height()),
                    "heightPixels", plist_object_integer(config_.display().height()),
                    "heightPhysical", plist_object_integer(0),
                    "width", plist_object_integer(config_.display().width()),
                    "widthPixels", plist_object_integer(config_.display().width()),
                    "widthPhysical", plist_object_integer(0),
                    "refreshRate", plist_object_real(config_.display().refreshRate()),
                    "overscanned", plist_object_false(),
                    "rotation", plist_object_true(),
                    "uuid", plist_object_string(config_.display().uuid().c_str())
                )
            )
        );

        res.with_status(ok)
            .with_content_type(APPLICATION_BINARY_PLIST)
            .with_content(info.to_bytes_array());
    }

    void ap_airplay_session::post_feedback_handler(const details::request& req, details::response& res)
    {
        res.with_status(ok);
    }

    void ap_airplay_session::record_handler(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok)
            .with_header("Audio-Latency", "3750");
    }

    void ap_airplay_session::get_parameter_handler(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok)
            .with_content_type(TEXT_PARAMETERS)
            .with_content("volume: 0.000000\r\n");
    }

    void ap_airplay_session::set_parameter_handler(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok);
    }

    void ap_airplay_session::teardown_handler(const details::request& req, details::response& res)
    {
        if (0 == req.content_type.compare(APPLICATION_BINARY_PLIST))
        {
            auto_plist data_obj = plist_object_from_bplist(req.body.data(), req.body.size());
            if (data_obj)
            {
                auto streams = plist_object_dict_get_value(data_obj, "streams");
                if (streams && PLIST_TYPE_ARRAY == plist_object_get_type(streams))
                {
                    auto stream_obj = plist_object_array_get_value(streams, 0);
                    if (stream_obj && PLIST_TYPE_DICT == plist_object_get_type(stream_obj))
                    {
                        auto type_obj = plist_object_dict_get_value(stream_obj, "type");
                        if (type_obj && PLIST_TYPE_INTEGER == plist_object_get_type(type_obj))
                        {
                            int64_t type = 0;
                            plist_object_integer_get_value(type_obj, &type);
                            if (stream_type_t::video == type)
                            {
                                // Stop video stream
                                //if (video_stream_service_) 
                                //{
                                //    video_stream_service_->stop();
                                //    video_stream_service_.reset();
                                //}
                            }
                            else if (stream_type_t::audio == type)
                            {
                                // Stop audio stream

                            }
                            else
                                LOGE() << "Unknown stream type";
                        }
                    }
                }
            }
        }

        res.with_status(ok);
    }

    void ap_airplay_session::flush_handler(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok)
            .with_content_type(APPLICATION_BINARY_PLIST);
    }

    void ap_airplay_session::get_server_info(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok);
    }

    void ap_airplay_session::post_reverse(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok);
    }

    void ap_airplay_session::post_play(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok);
    }

    void ap_airplay_session::post_scrub(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok);
    }

    void ap_airplay_session::post_rate(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok);
    }

    void ap_airplay_session::post_stop(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok);
    }

    void ap_airplay_session::post_action(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok);
    }

    void ap_airplay_session::get_playback_info(const details::request& req, details::response& res)
    {
        res.with_status(ok);
    }

    void ap_airplay_session::put_setProperty(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok);
    }

    void ap_airplay_session::get_getProperty(const details::request& req, details::response& res)
    {
        DUMP_REQUEST(req);

        res.with_status(ok);
    }

    void ap_airplay_session::post_receive_request_head()
    {
        asio::async_read_until(socket_, in_stream_, RNRN_LINE_BREAK,
            asio::bind_executor(
                strand_,
                std::bind(
                    &ap_airplay_session::on_request_head_received,
                    shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2))
        );
    }

    void ap_airplay_session::on_request_head_received(const asio::error_code& e, std::size_t bytes_transferred)
    {
        if (!e)
        {
            // Parse the request head
            std::string head_data(
                asio::buffers_begin(in_stream_.data()),
                asio::buffers_begin(in_stream_.data()) + bytes_transferred);
            in_stream_.consume(bytes_transferred);

            bool result = parser_.parse(request_, head_data);
            if (result)
            {
                if (request_.content_length)
                {
                    int body_bytes_to_read = request_.content_length - in_stream_.size();
                    if (0 == body_bytes_to_read)
                    {
                        if (in_stream_.size())
                        {
                            request_.body.clear();
                            request_.body.resize(request_.content_length, 0);

                            request_.body.assign(
                                asio::buffers_begin(in_stream_.data()),
                                asio::buffers_begin(in_stream_.data()) + in_stream_.size());
                            in_stream_.consume(in_stream_.size());
                        }

                        process_request();

                        post_receive_request_head();
                    }
                    else
                    {
                        post_receive_request_body();
                    }
                }
                else
                {
                    process_request();

                    post_receive_request_head();
                }
            }
            else
            {
                // Invalid request head, close this session
                close();
            }
        }
        else
        {
            error_handler(e);
        }
    }

    void ap_airplay_session::post_receive_request_body()
    {
        asio::async_read(socket_, in_stream_,
            std::bind(
                &ap_airplay_session::body_completion_condition,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2),
            asio::bind_executor(
                strand_,
                std::bind(
                    &ap_airplay_session::on_request_body_received,
                    shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2))
        );
    }

    void ap_airplay_session::on_request_body_received(const asio::error_code& e, std::size_t bytes_transferred)
    {
        if (!e)
        {
            request_.body.assign(
                asio::buffers_begin(in_stream_.data()),
                asio::buffers_begin(in_stream_.data()) + in_stream_.size());
            in_stream_.consume(in_stream_.size());

            process_request();
            post_receive_request_head();
        }
        else
        {
            error_handler(e);
        }
    }

    void ap_airplay_session::post_send_response(const details::response& res)
    {
        res.format_to_stream(out_stream_);
        asio::async_write(socket_, out_stream_,
            asio::bind_executor(
                strand_,
                std::bind(
                    &ap_airplay_session::on_response_sent,
                    shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2))
        );
    }

    void ap_airplay_session::on_response_sent(const asio::error_code& e, std::size_t bytes_transferred)
    {
        if (!e)
        {
            //LOGI() << ">>>>> " << bytes_transferred << " bytes sent successfully";
        }
        else
        {
            error_handler(e);
        }
    }

    void ap_airplay_session::add_common_header(const details::request& req, details::response& res)
    {
        res.with_header(HEADER_SERVER, "AirTunes/220.68")
            .with_header(HEADER_DATE, gmt_time_string());

        if (!req.cseq.empty())
            res.with_header(HEADER_CSEQ, req.cseq);

        if (0 != req.method.compare("RECORD") && 0 != req.method.compare("SET_PARAMETER"))
        {
            res.with_header(HEADER_AUDIO_JACK_STATUS, "Connected");
        }
    }

    void ap_airplay_session::error_handler(const asio::error_code& e)
    {
        LOGE() << "Failed to receive mirror header: [" << e.value() << "] " << e.message();

        switch (e.value())
        {
        case asio::error::eof:
        case asio::error::connection_reset:
        {
            //close();
        }
        break;
        case asio::error::connection_aborted:
        case asio::error::access_denied:
        case asio::error::address_family_not_supported:
        case asio::error::address_in_use:
        case asio::error::already_connected:
        case asio::error::connection_refused:
        case asio::error::bad_descriptor:
        case asio::error::fault:
        case asio::error::host_unreachable:
        case asio::error::in_progress:
        case asio::error::interrupted:
        case asio::error::invalid_argument:
        case asio::error::message_size:
        case asio::error::name_too_long:
        case asio::error::network_down:
        case asio::error::network_reset:
        case asio::error::network_unreachable:
        case asio::error::no_descriptors:
        case asio::error::no_buffer_space:
        case asio::error::no_protocol_option:
        case asio::error::not_connected:
        case asio::error::not_socket:
        case asio::error::operation_not_supported:
        case asio::error::shut_down:
        case asio::error::timed_out:
        case asio::error::would_block:
            break;
        }
    }

    std::size_t ap_airplay_session::body_completion_condition(const asio::error_code& error, std::size_t bytes_transferred)
    {
        return request_.content_length - in_stream_.size();
    }

    void ap_airplay_session::process_request()
    {
        // Dump the request
        //LOGD() << "<<<<< " << request_.method << " " << request_.uri << " " << request_.scheme_version;
        //LOGD() << "Header:";
        //for (auto& header : request_.headers)
        //    LOGD() << "      " << header.first << ": " << header.second;
        //LOGD() << "Content:";
        //LOGD() << "      " << request_.body.data();

        response res(request_.scheme_version);

        request_handler_map* handler_map = 0;
        if (0 == request_.scheme_version.find("RTSP"))
            handler_map = &rtsp_request_handlers_;
        else if (0 == request_.scheme_version.find("HTTP"))
            handler_map = &http_request_handlers_;
        else
            return;

        auto method_to_path = handler_map->find(request_.method);
        if (method_to_path == handler_map->end())
            method_not_found_handler(request_, res);
        else
        {
            auto path_to_handler = method_to_path->second.find("*");
            if (path_to_handler == method_to_path->second.end())
            {
                std::string path = request_.uri;
                auto index = request_.uri.find('?');
                if (std::string::npos != index)
                    path = request_.uri.substr(0, index);
                
                path_to_handler = method_to_path->second.find(path);
            }

            if (path_to_handler == method_to_path->second.end())
                path_not_found_handler(request_, res);
            else
                path_to_handler->second(request_, res);
        }

        add_common_header(request_, res);
        post_send_response(res);
    }

    void ap_airplay_session::register_request_handlers()
    {
        register_rtsp_request_handler(
            std::bind(&ap_airplay_session::options_handler,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "OPTIONS");

        register_rtsp_request_handler(
            std::bind(&ap_airplay_session::post_pair_setup_handler,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "POST", "/pair-setup");

        register_rtsp_request_handler(
            std::bind(&ap_airplay_session::post_pair_verify_handler,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "POST", "/pair-verify");

        register_rtsp_request_handler(
            std::bind(&ap_airplay_session::post_fp_setup_handler,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "POST", "/fp-setup");

        register_rtsp_request_handler(
            std::bind(&ap_airplay_session::setup_handler,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "SETUP", "*");

        register_rtsp_request_handler(
            std::bind(&ap_airplay_session::get_info_handler,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "GET", "/info");

        register_rtsp_request_handler(
            std::bind(&ap_airplay_session::post_feedback_handler,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "POST", "/feedback");

        register_rtsp_request_handler(
            std::bind(&ap_airplay_session::record_handler,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "RECORD", "*");

        register_rtsp_request_handler(
            std::bind(&ap_airplay_session::get_parameter_handler,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "GET_PARAMETER", "*");

        register_rtsp_request_handler(
            std::bind(&ap_airplay_session::set_parameter_handler,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "SET_PARAMETER", "*");

        register_rtsp_request_handler(
            std::bind(&ap_airplay_session::teardown_handler,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "TEARDOWN", "*");

        register_rtsp_request_handler(
            std::bind(&ap_airplay_session::flush_handler,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "FLUSH", "*");

        register_http_request_handler(
            std::bind(&ap_airplay_session::get_server_info,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "GET", "/server-info");

        register_http_request_handler(
            std::bind(&ap_airplay_session::post_reverse,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "POST", "/reverse");

        register_http_request_handler(
            std::bind(&ap_airplay_session::post_play,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "POST", "/play");

        register_http_request_handler(
            std::bind(&ap_airplay_session::post_scrub,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "POST", "/scrub");

        register_http_request_handler(
            std::bind(&ap_airplay_session::post_rate,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "POST", "/rate");

        register_http_request_handler(
            std::bind(&ap_airplay_session::post_stop,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "POST", "/stop");

        register_http_request_handler(
            std::bind(&ap_airplay_session::post_action,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "POST", "/action");

        register_http_request_handler(
            std::bind(&ap_airplay_session::get_playback_info,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "GET", "/playback-info");

        register_http_request_handler(
            std::bind(&ap_airplay_session::put_setProperty,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "PUT", "/setProperty");

        register_http_request_handler(
            std::bind(&ap_airplay_session::get_getProperty,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            "GET", "/getProperty");
    }

    void ap_airplay_session::method_not_found_handler(const details::request& req, details::response& res)
    {
        LOGI() << "***** Method Not Allowed " << request_.method << " " << request_.uri;

        // Method not found
        //res.with_status(method_not_allowed);
        res.with_status(ok);
    }

    void ap_airplay_session::path_not_found_handler(const details::request& req, details::response& res)
    {
        LOGI() << "***** Path Not Found " << request_.method << " " << request_.uri;

        // Path not found
        //res.with_status(not_found);
        res.with_status(ok);
    }

    ap_airplay_service::ap_airplay_service(ap_config& config, uint16_t port /*= 0*/)
        : tcp_service_base(port)
        , config_(config)
    {
    }

    ap_airplay_service::~ap_airplay_service()
    {
    }

    aps::network::tcp_session_ptr ap_airplay_service::prepare_new_session()
    {
        return std::make_shared<ap_airplay_session>(io_context(), config_);
    }
} }
