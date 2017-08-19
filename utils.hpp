#ifndef R_UTILS_HPP
#define R_UTILS_HPP

#include <iterator>
namespace rlib {
	template<typename _tData>
	void CheckedDelete(_tData *p)
	{
		if (p)
			delete p;
	}
	template<typename _tData>
	void CheckedDeleteArr(_tData *p)
	{
		if (p)
			delete[] p;
	}
	/* Deprecated.
	#include <functional>
	#include <utility>
	#define on_scope_exit(f) scope_exit_guard _guarder_id_##__COUNTER__(f);
	class scope_exit_guard
	{
	public:
		using namespace ::std;
		using func_t = std::function<void()>;
	private:
		func_t funcExit;
	public:
		explicit scope_exit_guard(func_t func) : funcExit(func) {}
		~scope_exit_guard() {funcExit();}
		scope_exit_guard() = delete;
		scope_exit_guard(const scope_exit_guard &) = delete;
		operator=(const scope_exit_guard &) = delete;
		scope_exit_guard(scope_exit_guard &&) = delete;
		operator=(scope_exit_guard &&) = delete;
	};*/
}

#endif
