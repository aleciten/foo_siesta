#pragma once
#include "stdafx.h"
#include <future>

class component {
public:	
	virtual bool handle(std::vector<string_t> paths, http::http_request msg) = 0;
	
	template <typename T>
	T in_main_thread(std::function<T(void)> func)
	{
		struct in_main : main_thread_callback
		{
			in_main(std::function<T(void)> func, std::promise<T>* promise) : m_func(func) {
				m_promise = promise;
			}

			void callback_run() override
			{
				T ret = m_func();
				m_promise->set_value(ret);
			}

			std::function<T(void)> m_func;
			std::promise<T>* m_promise;
		};

		std::promise<T> prom;
		auto callback = new service_impl_t<in_main>(func, &prom);

		static_api_ptr_t<main_thread_callback_manager>()->add_callback(callback);

		auto fut = prom.get_future();
		fut.wait();

		return fut.get();
	}
};

class playlists_component : public component {
public:
	bool handle(std::vector<string_t> paths, http::http_request msg);
	bool get_all(bool include_contents, http::http_request msg);
	bool get_active(http::http_request msg);
	bool get_index(int index, http::http_request msg);
};

class control_component : public component {
public:
	bool handle(std::vector<string_t> paths, http::http_request msg);
	//bool get_active(http::http_request msg);

	bool play(playback_control::t_track_command cmd, http::http_request msg);
	bool stop(http::http_request msg);
	bool pause(http::http_request msg);
	bool next(http::http_request msg);
	bool prev(http::http_request msg);
	bool get_vol(http::http_request msg);
	bool set_vol(float vol, http::http_request msg);

private:
	typedef static_api_ptr_t<play_control> pc_ptr;

	template<typename T>
	T pc_do(std::function<T(pc_ptr)> func) {
		return in_main_thread(std::function<T()>([func]() -> T {
			pc_ptr pc;
			return func(pc);
		}));
	}
};

class state_component : public component {
public:
	bool handle(std::vector<string_t> paths, http::http_request msg);
	bool get_state(http::http_request msg);
};

class ui_component : public component {
public:
	bool handle(std::vector<string_t> paths, http::http_request msg);
	bool get(http::http_request msg);
};

template<typename R>
class factory_registry {
public:
	struct key_in_use_exception : public std::exception {};
	
	template<typename T>
	void define(string_t key) {
        if (m_registry.find(key) != m_registry.end()) throw key_in_use_exception();
        m_registry[key] = constructor_func([]() -> ret_type { return ret_type(new T); });
	}

	shared_ptr<R> create(string_t name) {
        if (m_registry.find(name) == m_registry.end())
			return NULL; 
		
        return m_registry[name]();
	}

private:
	typedef shared_ptr<R> ret_type;
	typedef std::function<ret_type()> constructor_func;
	typedef std::map<string_t, constructor_func> registry_map;

	registry_map m_registry;
};

class component_factory : public factory_registry<component> {};