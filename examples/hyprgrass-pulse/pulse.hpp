#pragma once

#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/thread-mainloop.h>
#include <pulse/volume.h>

#include <functional>
#include <memory>
#include <string>

enum class ChangeType {
    Increase,
    Decrease,
};

class AudioBackend {
  private:
    static void subscribeCb(pa_context*, pa_subscription_event_type_t, uint32_t, void*);
    static void contextStateCb(pa_context*, void*);
    static void sinkInfoCb(pa_context*, const pa_sink_info*, int, void*);
    static void sourceInfoCb(pa_context*, const pa_source_info* i, int, void* data);
    static void serverInfoCb(pa_context*, const pa_server_info*, void*);
    static void volumeModifyCb(pa_context*, int, void*);
    void connectContext();

    pa_threaded_mainloop* mainloop_;
    pa_mainloop_api* mainloop_api_;
    pa_context* context_;
    pa_cvolume pa_volume_;

    // SINK
    uint32_t sink_idx_{0};
    uint16_t volume_;
    bool muted_;
    std::string port_name_;
    std::string form_factor_;
    std::string desc_;
    std::string monitor_;
    std::string current_sink_name_;
    std::string default_sink_name;
    bool default_sink_running_;
    bool current_sink_running_;
    // SOURCE
    uint32_t source_idx_{0};
    uint16_t source_volume_;
    bool source_muted_;
    std::string source_port_name_;
    std::string source_desc_;
    std::string default_source_name_;

    std::vector<std::string> ignored_sinks_;

    std::function<void()> on_updated_cb_ = []() {};

    /* Hack to keep constructor inaccessible but still public.
     * This is required to be able to use std::make_shared.
     * It is important to keep this class only accessible via a reference-counted
     * pointer because the destructor will manually free memory, and this could be
     * a problem with C++20's copy and move semantics.
     */
    struct private_constructor_tag {};

  public:
    static std::shared_ptr<AudioBackend> getInstance(std::function<void()> on_updated_cb = []() {});

    AudioBackend(std::function<void()> on_updated_cb, private_constructor_tag tag);
    ~AudioBackend();

    void changeVolume(uint16_t volume, uint16_t min_volume = 0, uint16_t max_volume = 100);
    void changeVolume(ChangeType change_type, double step = 1, uint16_t max_volume = 100);
};
