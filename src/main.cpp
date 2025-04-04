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

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/connect.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

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

#include <memory>
#include <chrono>
#include <string>
#include <string_view>
#include <iostream>
#include <functional>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <locale.h>
#endif // _WIN32

namespace net {
using namespace boost::asio ;
using namespace boost::beast;
      namespace http = boost::beast::http;
using                  boost::asio::ip::tcp;
}

#define   PORT "80"
#define   HOST "api.openbudget.gov.ua"
#define TARGET "/api/public/"
#define   PING "ping"
#define    API "localBudgetData"
#define     P0 "?budgetCode=2555900000"
#define     P1 "&budgetItem=INCOMES"
#define     P2 "&period=QUARTER"
#define     P3 "&year=2023"

//constexpr auto get = "https://api.openbudget.gov.ua/api/public/localBudgetData?budgetCode=2555900000&budgetItem=INCOMES&period=QUARTER&year=2024";

// Performs an HTTP GET and prints the response
class Session : public std::enable_shared_from_this<Session>
{
	net::http::request <net::http:: empty_body> request {};
	net::http::response<net::http::string_body> response{};
	net::tcp::resolver resolver;
	net::tcp_stream tstream;
	net::flat_buffer buffer{}; // (Must persist between reads)
	std::chrono::steady_clock::duration timeout{};

private:
	static void fail(net::error_code ec, char const* what) { std::cerr << what << ": " << ec.message() << '\n'; } // Report a failure

public:
	// Objects are constructed with a strand to ensure that handlers do not execute concurrently
	explicit Session(net::io_context &ioc) : resolver(net::make_strand(ioc)), tstream(net::make_strand(ioc)) {}

	// Queue the asynchronous operation // queue async I/O operation
	void Set(std::string_view host, std::string_view port, std::string_view target, std::chrono::steady_clock::duration timeout = std::chrono::seconds{0}) {
		this->timeout = timeout;
		//this->tstream.expires_never();

		// Set up an HTTP GET request message
		request.set(net::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		request.set(net::http::field::host, host);
		request.method(net::http::verb::get);
		request.version(11);
		request.target (target);

		// Look up the domain name
		resolver.async_resolve(host, port,	std::bind_front(&Session::OnResolve, shared_from_this())); // queue async I/O operation
	}

	void OnResolve(net::error_code ec, net::tcp::resolver::results_type resolves) {
		using namespace std::literals::chrono_literals;
		if (ec) return fail(ec, "resolve");
		if (timeout != 0s) tstream.expires_after(timeout); // Set a timeout on the operation
		tstream.async_connect(resolves, std::bind_front(&Session::OnConnect, shared_from_this())); // Make the connection on the IP address we get from a lookup
	}

	void OnConnect(net::error_code ec, net::tcp::resolver::results_type::endpoint_type) {
		if (ec) return fail(ec, "connect");
		net::http::async_write(tstream, request, std::bind_front(&Session::OnWrite, shared_from_this())); // Send the HTTP request to the remote host
	}

	void OnWrite(net::error_code ec, size_t) {
		if (ec) return fail(ec, "write");
		net::http::async_read(tstream, buffer, response, std::bind_front(&Session::OnRead, shared_from_this())); // Receive the HTTP response
	}

	void OnRead(net::error_code ec, size_t) {
		if (ec) return fail(ec, "read");
		tstream.socket().shutdown(net::tcp::socket::shutdown_both, ec); // Gracefully close the socket

		std::cout << response << std::endl; // Write the message to standard out

		if (ec && ec != net::errc::not_connected) return fail(ec, "shutdown");
	}
};

int main()
{
#ifdef _WIN32 // console UTF-8
	//system("chcp 65001");
	setlocale(LC_CTYPE, ".UTF8");
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP      (CP_UTF8);
#endif

	// The io_context is required for all I/O
	net::io_context ioc;

	// Launch the asynchronous operation
	using namespace std::literals::chrono_literals;
	  std::make_shared<Session>(ioc)->Set(HOST, PORT, TARGET PING           , 4s);
	//std::make_shared<Session>(ioc)->Set(HOST, PORT, TARGET API P0 P1 P2 P3, std::chrono::seconds{5});
	ioc.run(); // Dequeue and execute I/O operations. The call will return when the get operation is complete
}
