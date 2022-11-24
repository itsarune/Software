#include "software/logger/plotjuggler_sink.h"

#include <google/protobuf/util/json_util.h>

#include "shared/constants.h"

PlotJugglerSink::PlotJugglerSink() : io_service(), socket_(io_service)
{
    boost::asio::ip::address addr =
        boost::asio::ip::make_address(PLOTJUGGLER_DEFAULT_HOST);

    // The receiver endpoint identifies where this UdpSender will send data to
    receiver_endpoint = boost::asio::ip::udp::endpoint(addr, PLOTJUGGLER_DEFAULT_PORT);

    socket_.open(receiver_endpoint.protocol());

    io_service_thread = std::thread([this]() { io_service.run(); });
}

PlotJugglerSink::~PlotJugglerSink()
{
    // Stop the io_service. This is safe to call from another thread.
    // https://stackoverflow.com/questions/4808848/boost-asio-stopping-io-service
    // This MUST be done before attempting to join the thread because otherwise the
    // io_service will not stop and the thread will not join
    io_service.stop();

    // Join the io_service_thread so that we wait for it to exit before destructing the
    // thread object. If we do not wait for the thread to finish executing, it will call
    // `std::terminate` when we deallocate the thread object and kill our whole program
    io_service_thread.join();

    socket_.close();
}

void PlotJugglerSink::sendToPlotJuggler(g3::LogMessageMover log_entry)
{
    if (log_entry.get()._level.value == PLOTJUGGLER.value)
    {
        data_buffer = log_entry.get().message();
        socket_.send_to(boost::asio::buffer(data_buffer), receiver_endpoint);
        std::cout << "Sending: " << data_buffer << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os,
                         const TbotsProto::PlotJugglerValue& plotjuggler_value)
{
    std::string json_string;

    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = false;  // TODO: Added to avoid line breaks in g3log
    options.always_print_primitive_fields = true;
    options.preserve_proto_field_names    = true;

    MessageToJsonString(plotjuggler_value, &json_string, options);

    os << json_string;
    return os;
}
