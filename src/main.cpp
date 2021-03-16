#include <cstdio>
#include <cstdint>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <QApplication>

#include "http.hpp"
#include "mainwindow.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    if (argc < 2) {
        std::fprintf(stderr, "Usage: %s website\n", argv[0]);
        return EXIT_FAILURE;
    }

    auto client = snatch::HttpsSyncClient();

    auto host = argv[1];
    if (auto ec = client.connect(host); ec)
        std::fprintf(stderr, "Failed to connect: %s\n", ec.message().c_str());

    {
        auto &&[ec, resp] = client.get("/");
        if (ec)
            std::fprintf(stderr, "Failed to GET: %s\n", ec.message().c_str());

        std::printf("Response status: %u\n", static_cast<std::uint32_t>(resp.result()));
        std::printf("Headers\n");
        for (const auto &header: resp)
            std::printf("  %.*s: %.*s\n", static_cast<int>(header.name_string().size()), header.name_string().data(),
                static_cast<int>(header.value().size()), header.value().data());

        std::printf("Body size: %#lx\n", resp.body().size());
        std::printf("Body:\n");
        std::puts(resp.body().c_str());
    }

    {
        auto &&[ec, resp] = client.head("/");
        if (ec)
            std::fprintf(stderr, "Failed to HEAD: %s\n", ec.message().c_str());

        std::printf("Response status: %u\n", static_cast<std::uint32_t>(resp.result()));
        // std::printf("Body size: %#lx\n", resp.body().size());
    }

    {
        auto &&[ec, resp] = client.post("test", "/");
        if (ec)
            std::fprintf(stderr, "Failed to POST: %s\n", ec.message().c_str());

        std::printf("Response status: %u\n", static_cast<std::uint32_t>(resp.result()));
        std::printf("Body size: %#lx\n", resp.body().size());
        std::puts(resp.body().c_str());
    }

    if (auto ec = client.shutdown(); ec)
        std::fprintf(stderr, "Failed to shutdown: %s\n", ec.message().c_str());






    // QApplication app(argc, argv);

    // QIcon icon;
    // icon.addFile(":/Icons/AppIcon32");
    // icon.addFile(":/Icons/AppIcon128");
    // app.setWindowIcon(icon);

    // MainWindow main;
    // main.show();

    // return app.exec();
}
