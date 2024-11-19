#pragma once

#include <uv.h>

#include <fan/types/print.h>

#include <coroutine>
#include <stdexcept>
#include <memory>

namespace fan {
  namespace network {

    struct error_code_t {
      int code;
      constexpr error_code_t(int code) noexcept : code(code) {}
      constexpr operator int() const noexcept { return code; }
      void throw_if() const { if (code) throw code; }
      constexpr bool await_ready() const noexcept { return true; }
      constexpr void await_suspend(std::coroutine_handle<>) const noexcept {}
      void await_resume() const { throw_if(); }
    };

    struct tcp_t;
    struct getaddrinfo_t {
      struct getaddrinfo_data_t {
        uv_getaddrinfo_t getaddrinfo_handle;
        std::coroutine_handle<> co_handle;
        bool ready{ false };
        int status;
      };
      std::unique_ptr<getaddrinfo_data_t> data;

      getaddrinfo_t(const char* node, const char* service, struct addrinfo* hints = nullptr) :
        data(std::make_unique<getaddrinfo_data_t>()) {
        data->getaddrinfo_handle.data = data.get();
        uv_getaddrinfo(uv_default_loop(), &data->getaddrinfo_handle, [](uv_getaddrinfo_t* getaddrinfo_handle, int status, struct addrinfo* res) {
          auto data = static_cast<getaddrinfo_data_t*>(getaddrinfo_handle->data);
          if (status == UV_ECANCELED) {
            delete data;
            return;
          }
          data->ready = true;
          data->status = status;
          [[likely]] if (data->co_handle) {
            data->co_handle();
          }
        }, node, service, hints);
      }

      bool await_ready() const { return data->ready; }
      void await_suspend(std::coroutine_handle<> h) { data->co_handle = h; }
      int await_resume() const { return data->status; };

      ~getaddrinfo_t() {
        if (data) {
          uv_freeaddrinfo(data->getaddrinfo_handle.addrinfo);
          if (uv_cancel(reinterpret_cast<uv_req_t*>(&data->getaddrinfo_handle)) == 0) {
            data.release();
          }
        }
      }
    };

    struct writer_t {
      struct writer_data_t {
        std::shared_ptr<uv_stream_t> stream;
        uv_write_t write_handle;
        std::string to_write;
        std::coroutine_handle<> co_handle;
        int status{ 0 };
      };

      std::unique_ptr<writer_data_t> data;

      template <typename T>
        requires (std::is_same_v<T, tcp_t>)
      writer_t(const T& tcp) :
        data(new writer_data_t{std::reinterpret_pointer_cast<uv_stream_t>(tcp.socket)}) {
        data->write_handle.data = data.get();
      }
      writer_t(writer_t&&) = default;
      writer_t& operator=(writer_t&&) = default;

      int write(std::string some_data) {
        data->to_write = std::move(some_data);
        data->status = 1;
        uv_buf_t buf = uv_buf_init(data->to_write.data(), data->to_write.size());
        int r = uv_write(&data->write_handle, data->stream.get(), &buf, 1,
          [](uv_write_t* write_handle, int status) {
            auto data = static_cast<writer_data_t*>(write_handle->data);
            data->status = status;
            if (data->co_handle) {
              data->co_handle();
            }
          });
        if (r < 0) {
          fan::throw_error("tcp write failed", r);
        }
        return r;
      }

      bool await_ready() const noexcept { return data->status <= 0; }
      void await_suspend(std::coroutine_handle<> h) noexcept { data->co_handle = h; }
      int await_resume() noexcept {
        data->co_handle = nullptr;
        return data->status;
      }

      ~writer_t() {
        if (data && data->status == 1)
        {
          data->write_handle.cb = [](uv_write_t* write_handle, int) {
            delete static_cast<writer_data_t*>(write_handle->data);
            };
          data.release();
        }
      }
    };

    struct connector_t {
      struct connector_data_t {
        uv_connect_t req;
        std::coroutine_handle<> co_handle;
        int status;
      };

      std::unique_ptr<connector_data_t> data;

      template <typename T>
      requires (std::is_same_v<T, tcp_t>)
      connector_t(const T& tcp, const char* ip, int port) :
        data{ std::make_unique<connector_data_t>() }
      {
        data->req.data = data.get();
        struct sockaddr_in client_addr;
        auto result = uv_ip4_addr(ip, port, &client_addr);

        if (result != 0) {
          fan::throw_error("failed to resolve address");
        }

        data->status = uv_tcp_connect(&data->req, tcp.socket.get(), reinterpret_cast<sockaddr*>(&client_addr),
          [](uv_connect_t* req, int status) {
            auto* data = static_cast<connector_data_t*>(req->data);
            data->status = status;
            [[likely]] if (data->co_handle) {
              data->co_handle();
            }
        });
        if (data->status == 0) {
          data->status = 1;
        }
      }

      template <typename T>
      requires (std::is_same_v<T, tcp_t>)
      connector_t(const T& tcp, const getaddrinfo_t& info) : data(std::make_unique<connector_data_t>()) {
        data->req.data = data.get();
        data->status = uv_tcp_connect(
          &data->req, 
          tcp.socket.get(), 
          (const struct sockaddr*)info.data->getaddrinfo_handle.addrinfo->ai_addr,
          [](uv_connect_t* req, int status) {
            auto* data = static_cast<connector_data_t*>(req->data);
            data->status = status;
            [[likely]] if (data->co_handle) {
              data->co_handle();
            }
          }
        );

        if (data->status == 0) {
          data->status = 1;
        }
      }

      connector_t(connector_t&&) = default;
      connector_t& operator=(connector_t&&) = default;
      ~connector_t() {
        [[unlikely]] if (data && data->status == 1) {
          data->req.cb = [](uv_connect_t* req, int) {
            delete static_cast<connector_data_t*>(req->data);
            };
          data.release();
        }
      }

      bool await_ready() { return data->status <= 0; }
      void await_suspend(std::coroutine_handle<> h) { data->co_handle = h; }
      int await_resume() {
        if (data->status != 0) {
          throw std::runtime_error(std::string("connection failed with") + uv_strerror(data->status));
        }
        return data->status;
      }
    };

    struct reader_t {
      std::shared_ptr<uv_stream_t> stream;
      std::string buf;
      ssize_t nread{ 0 };
      std::coroutine_handle<> co_handle;

      template <typename T>
      requires (std::is_same_v<T, tcp_t>)
      reader_t(const T& tcp) : stream{ std::reinterpret_pointer_cast<uv_stream_t>(tcp.socket) } {
        stream->data = this;
      }
      reader_t(const reader_t&) = delete;
      reader_t(reader_t&& r) noexcept :
        stream{ std::move(r.stream) },
        buf{ std::move(r.buf) },
        nread{ std::move(r.nread) },
        co_handle{ std::move(r.co_handle) } {
        stream->data = this;
      }

      void stop() {
        uv_read_stop(stream.get());
      }
      int start() noexcept {
        return uv_read_start(stream.get(),
          [](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
            auto self = static_cast<reader_t*>(handle->data);
            self->buf.resize(suggested_size);
            *buf = uv_buf_init(self->buf.data(), suggested_size);
          },
          [](uv_stream_t* req, ssize_t nread, const uv_buf_t* buf) {
            auto self = static_cast<reader_t*>(req->data);
            self->nread = nread;
            [[likely]] if (self->co_handle)
              self->co_handle();
          }
        );
      }
      bool await_ready() const { return nread > 0; }
      void await_suspend(std::coroutine_handle<> h) { co_handle = h; }
      const char* await_resume() {
        const char* out = nullptr;
        if (nread > 0) {
          out = buf.c_str();
          nread = 0;
        }
        co_handle = nullptr;
        return out;
      }

      ~reader_t() {
        if (stream) {
          uv_read_stop(stream.get());
        }
      }
    };

    struct listener_t {
      std::shared_ptr<uv_stream_t> stream;
      std::coroutine_handle<> co_handle;
      int status;
      bool ready;

      template <typename T>
      requires (std::is_same_v<T, tcp_t>)
      listener_t(const T& tcp, int backlog) :
        stream{ std::reinterpret_pointer_cast<uv_stream_t>(tcp.socket) },
        ready{ false }
      {
        stream->data = this;
        auto r = uv_listen(stream.get(), backlog, [](uv_stream_t* req, int status) {
          auto self = static_cast<listener_t*>(req->data);
          self->status = status;
          self->ready = true;
          if (self->co_handle)
            self->co_handle();
          });
        if (r != 0) {
          fan::print("listen error", r);
        }
      }

      listener_t(listener_t&& l) :
        stream(std::move(l.stream)),
        co_handle(std::move(l.co_handle)),
        status(l.status),
        ready(l.ready) {
        stream->data = this;
      }

      bool await_ready() const { return ready; }
      void await_suspend(std::coroutine_handle<> h) { co_handle = h; }
      int await_resume() {
        ready = false;
        co_handle = nullptr;
        return status;
      }
    };

    struct tcp_t {
      std::shared_ptr<uv_tcp_t> socket;

      struct tcp_deleter_t {
        void operator()(void* p) const {
          uv_close(static_cast<uv_handle_t*>(p), [](uv_handle_t* req) {
            delete reinterpret_cast<uv_tcp_t*>(req);
          });
        }
      };

      tcp_t() : socket(new uv_tcp_t, tcp_deleter_t{}) {
        uv_tcp_init(uv_default_loop(), socket.get());
      }
      tcp_t(const tcp_t&) = delete;
      tcp_t& operator=(const tcp_t&) = delete;
      tcp_t(tcp_t&&) = default;
      tcp_t& operator=(tcp_t&&) = default;

      error_code_t accept(tcp_t& client) noexcept {
        return uv_accept(reinterpret_cast<uv_stream_t*>(socket.get()),
          reinterpret_cast<uv_stream_t*>(client.socket.get()));
      }
      error_code_t bind(std::string ip, int port) noexcept {
        struct sockaddr_in bind_addr;
        uv_ip4_addr(ip.c_str(), port, &bind_addr);
        return uv_tcp_bind(socket.get(), reinterpret_cast<sockaddr*>(&bind_addr), 0);
      }
      listener_t listen(int backlog) {
        return listener_t{ *this, backlog };
      }
      connector_t connect(const char* ip, int port) {
        return connector_t{ *this, ip, port };
      }
      connector_t connect(const getaddrinfo_t& info) {
        return connector_t{ *this, info };
      }
      reader_t read() {
        reader_t r{ *this };
        r.start();
        return r;
      }
      writer_t write(std::string data)
      {
        writer_t w{ *this };
        w.write(std::move(data));
        return w;
      }
    };

    struct task_promise_t {
      std::coroutine_handle<> _continuation;
      std::exception_ptr _exception;

      task_promise_t() : _continuation{ std::noop_coroutine() } { }
      task_promise_t(const task_promise_t&) = delete;
      task_promise_t(task_promise_t&&) = default;

      std::coroutine_handle<task_promise_t> get_return_object() noexcept {
        return std::coroutine_handle<task_promise_t>::from_promise(*this);
      }
      std::coroutine_handle<> continuation() const noexcept { return _continuation; }
      void set_continuation(std::coroutine_handle<> h) noexcept { _continuation = h; }
      std::suspend_never initial_suspend() { return {}; }
      auto final_suspend() noexcept {
        struct final_awaiter_t {
          bool await_ready() const noexcept { return false; }
          auto await_suspend(std::coroutine_handle<task_promise_t> ch) const noexcept { return ch.promise()._continuation; }
          void await_resume() const noexcept {}
        };
        return final_awaiter_t{};
      }
      void unhandled_exception() { _exception = std::current_exception(); }
      void return_void() {}
    };

    struct task_t {
      using promise_type = task_promise_t;

      task_t(std::coroutine_handle<task_promise_t> ch) : _handle(ch) { }
      task_t(const task_t&) = delete;
      task_t(task_t&& f) : _handle(f._handle) {
        f._handle = nullptr;
      }
      task_t& operator=(task_t&& f) {
        destroy();
        std::swap(_handle, f._handle);
        return *this;
      }
      void destroy() {
        if (_handle) {
          _handle.destroy();
          _handle = nullptr;
        }
      }
      ~task_t() { if (_handle) { _handle.destroy(); } }

      bool valid() const { return static_cast<bool>(_handle); }
      bool await_ready() const noexcept {
        return _handle.done();
      }
      void await_suspend(std::coroutine_handle<> ch) noexcept {
        _handle.promise()._continuation = ch;
      }
      void await_resume() {
        if (_handle.promise()._exception) {
          std::rethrow_exception(_handle.promise()._exception);
        }
      }

      std::coroutine_handle<task_promise_t> _handle;
    };
  }
}