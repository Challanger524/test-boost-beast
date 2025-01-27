#ifndef PROJECT_CONFIG_PCH
#error "config.hpp" not force included // See "file:///./config.hpp" for details
#endif

// Order matters: `__GNUC__` defined for both Clang and GCC, so `__clang__` should go first
#if   defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Weverything" // disables everything on Clang, but not everything on GCC
#elif defined(__GNUG__) || defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wall"
#  pragma GCC diagnostic ignored "-Wextra"
#  pragma GCC diagnostic ignored "-Wpedantic"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#  pragma GCC diagnostic ignored "-Woverloaded-virtual"
#  pragma GCC diagnostic ignored "-Weffc++" // GCC only
#elif defined(_MSC_VER)
// Ignoring warnings from system headers is only supported with MSVC with the Ninja generators as of CMake 3.22 and the Visual Studio generators as of CMake 3.24. Of course, an MSVC of at least (compiler) version 19.29.30036.3 is required (the 14.29 toolchain in the Visual Studio installer).
// https://discourse.cmake.org/t/marking-headers-as-system-does-not-suppress-warnings-on-windows/6415/2?u=int_main
//#  pragma warning (push)
//#  pragma warning (disable: 4091)
#endif

// clang: -Wno-unused-command-line-argument // asm: context (from boost/asio)
#include <boost/asio.hpp>
#include <boost/beast.hpp>

#if   defined(__GNUG__) || defined(__GNUC__) || defined(__clang__)
#  pragma GCC diagnostic pop // alias in Clang (but "#pragma clang diagnostic *" will be ignored by GCC)
#elif defined(_MSC_VER)
//#  pragma warning (pop)
#endif

/*
#if   defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUG__) || defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
*/

#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <locale.h>
#endif // _WIN32

namespace asio  = boost::asio;
namespace beast = boost::beast;

#define   PORT "80"
#define   HOST "api.openbudget.gov.ua"
#define TARGET "/api/public/localBudgetData"
#define     P0 "?budgetCode=2555900000"
#define     P1 "&budgetItem=INCOMES"
#define     P2 "&period=QUARTER"
#define     P3 "&year=2024"

//constexpr auto get = "https://api.openbudget.gov.ua/api/public/localBudgetData?budgetCode=2555900000&budgetItem=INCOMES&period=QUARTER&year=2024";

int main()
{
#ifdef _WIN32 // console UTF-8
	//system("chcp 65001");
	setlocale(LC_CTYPE, ".UTF8");
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP      (CP_UTF8);
#endif

	try
	{
		asio::io_context                 context;
		asio::ip::tcp::resolver resolver(context);
		beast::tcp_stream       stream  (context);

		beast::http::request<beast::http::empty_body> request;
		request.method(beast::http::verb::get);
		request.version(11);
		/*
		request.target(TARGET P0 P1 P2 P3);*/
		request.target("/api/public/localBudgetData?budgetCode=2555900000&budgetItem=INCOMES&period=QUARTER&year=2024");
		request.set(beast::http::field::host      , HOST);
		request.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		request.keep_alive(false);

		const auto     resolved = resolver.resolve(HOST, PORT);
		stream.connect(resolved);
		beast::http::write(stream, request);

		/*
		beast::http::response<beast::http::   file_body> response;*/
		beast::http::response<beast::http::dynamic_body> response;
		beast::flat_buffer buffer;

		beast::http::read(stream, buffer, response);
		stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);

		std::cout << response << std::endl;
	}
	catch (std::exception& e) { std::cerr << "Exception: " << e.what() << "\n"; }

}
