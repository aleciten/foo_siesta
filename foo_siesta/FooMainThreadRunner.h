#include <functional>
#include <future>

template<class T>
class service_impl_t;
class main_thread_callback;
class main_thread_callback_manager;

template <typename TRet>
class MainThreadRunner {
public:
	MainThreadRunner(std::function<TRet(void)> func) : _func(func)
	{
	}

	std::future<TRet> run(bool async = false)
	{
		struct thread_callback : main_thread_callback
		{            
			thread_callback(std::function<TRet(void)> func, std::promise<TRet> *prom) : func(func) {
				promise = prom;
			}
            
            ~thread_callback() {
                delete promise;
            }

			void callback_run() override
			{
				TRet ret = func();                
                promise->set_value(ret);
			}

			std::function<TRet(void)> func;
			std::promise<TRet> *promise;
		};

		std::promise<TRet> *prom = new std::promise<TRet>();
		auto callback = new service_impl_t<thread_callback>(_func, prom);

		static_api_ptr_t<main_thread_callback_manager>()->add_callback(callback);

		return prom->get_future();
	}

private:
	std::function<TRet(void)> _func;
};
