#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <google/protobuf/stubs/common.h>
#include <FysBus.hh>
#include <Game.hh>
#include <BusListener.hh>
#include <FySMessage.pb.h>
#include <listener/GamingListener.hh>
#include <boost/asio/io_service.hpp>
#include <Context.hh>
#include <MemoryPool.hpp>
#include <Sprite.hh>


static const std::string welcomeMsg =
    "                                                                                                                                                               \n"
    "                                                                                                                                                               \n"
    "                             *(%%&&&&&&&&&&&%%#/.                                                              ,(#%&&&&&&&&&&&&%#(,                            \n"
    "                        .(&&&&&&&&&&&&&&&&&&&&&&&&&%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%&&&&&&&&&&&&&&&&&&&&&&&&%*                        \n"
    "                     .(&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%(                     \n"
    "                   .%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&(                   \n"
    "                  ,&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&                  \n"
    "                 *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%                 \n"
    "                 %&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&(                \n"
    "                *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%                \n"
    "                %&&&&&&&&&&&&&&&&&&&&&&%,     .#&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%,       (%&&&&&&&&&&&&&&&&&&&&&/               \n"
    "               /&&&&&&&&&&&&&&&&&&&&&%*         ,&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%.           *&&&&&&&&&&&&&&&&&&&&%*              \n"
    "              ,%&&&&&&&&&&&&&&&&&&&&&*           (&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&,             /&&&&&&&&&&&&&&&&&&&&%              \n"
    "              #&&&&&&&&&&&&&&&&&&&&&&*           /&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%              .%&&&&&&&&&&&&&&&&&&&&*             \n"
    "             ,&&&&&&&&&&&&&&&&&&&&&&&*           /&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&,             /&&&&&&&&&&&&&&&&&&&&&%             \n"
    "             #&&&&&&&&&&&&&&&&&&&&&&&*           /&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%,           *&&&&&&&&&&&&&&&&&&&&&&&*            \n"
    "            ,&&&&&&&&&&&&&,                             *%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%,      .(%&&&&&&&&&&&&&&&&&&&&&&&&%            \n"
    "            #&&&&&&&&&&&&%.                                  %&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%%%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*           \n"
    "           ,&&&&&&&&&&&&%.                                    %&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%(***(%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%           \n"
    "           %&&&&&&&&&&&&%                                     #&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%.         ,%&&&&&&&&&&&&&&&&%#%%&&&&&&&&&&&&&&&&&&&*          \n"
    "          ,&&&&&&&&&&&&&&/                                   *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&             #&&&&&&&&&&&&*        ,%&&&&&&&&&&&&&&&%          \n"
    "          #&&&&&&&&&&&&&&.                               .(&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&.             ,&&&&&&&&&&(            (&&&&&&&&&&&&&&&*         \n"
    "         *&&&&&&&&&&&&&&&&&&&%%%%%%%%*           /%%%%%%%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&              .&&&&&&&&              %&&&&&&&&&&&&&&%         \n"
    "         %&&&&&&&&&&&&&&&&&&&&&&&&&&&*           /&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&             #&&&&&&&&&/              #&&&&&&&&&&&&&&&/        \n"
    "        *&&&&&&&&&&&&&&&&&&&&&&&&&&&&*           /&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%.         .%&&&&&&&&&&%             .%&&&&&&&&&&&&&&&%        \n"
    "        %&&&&&&&&&&&&&&&&&&&&&&&&&&&&*           /&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%(.   .(%&&&&&&&&&&&&.           %&&&&&&&&&&&&&&&&&*       \n"
    "       ,&&&&&&&&&&&&&&&&&&&&&&&&&&&&           %&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%*      ,%&&&&&&&&&&&&&&&&&&&%,      \n"
    "      .%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%,       .%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&      \n"
    "      #&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%#(#%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&,     \n"
    "     .&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     \n"
    "     #&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&,    \n"
    "    ,&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%    \n"
    "    (&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&,   \n"
    "   .&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%   \n"
    "   #&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&,  \n"
    "  ,&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%,                                                                                  ,&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%  \n"
    "  (&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%.                                                                                    *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&, \n"
    " .&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%                                                                                       ,&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&% \n"
    " #&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&                                                                                         .%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&,\n"
    " #&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&                                                                                           .%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*\n"
    " *&&&&&&&&&&&&&&&&&&&&&&&&&&&&&                                                                                             .%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%.\n"
    "  (&&&&&&&&&&&&&&&&&&&&&&&&&&&&*                                                                                                #&&&&&&&&&&&&&&&&&&&&&&&&&&&&. \n"
    "   ,%&&&&&&&&&&&&&&&&&&&&&&&&/                                                                                                    %&&&&&&&&&&&&&&&&&&&&&&&   \n"
    "     ,%&&&&&&&&&&&&&&&&&&&&(                                                                                                       .%&&&&&&&&&&&&&&&&&&&&(     \n"
    "        ,#&&&&&&&&&&&&&&%*                                                                                                            (&&&&&&&&&&&&&&%(.       \n"
    "              .,*//*.                                                                                                                     .,*//*,.             \n"
    "                                                                                                           \n"
    "                                                                                                           \n"
    "8888888888                    Y88b   d88P                          .d8888b.                    888 \n"
    "888                            Y88b d88P                          d88P  Y88b                   888 \n"
    "888                             Y88o88P                           Y88b.                        888 \n"
    "8888888 888d888  .d88b.   .d88b. Y888P   .d88b.  888  888 888d888  \"Y888b.    .d88b.  888  888 888 \n"
    "888     888P\"   d8P  Y8b d8P  Y8b 888   d88\"\"88b 888  888 888P\"       \"Y88b. d88\"\"88b 888  888 888 \n"
    "888     888     88888888 88888888 888   888  888 888  888 888           \"888 888  888 888  888 888 \n"
    "888     888     Y8b.     Y8b.     888   Y88..88P Y88b 888 888     Y88b  d88P Y88..88P Y88b 888 888 \n"
    "888     888      \"Y8888   \"Y8888  888    \"Y88P\"   \"Y88888 888      \"Y8888P\"   \"Y88P\"   \"Y88888 888 \n"
    " \n";

using namespace fys::mq;
using namespace fys::cl;

using GamingListener = BusListener <buslistener::GamingListener, FysBus<fys::pb::FySMessage, 2>>;

void welcome(bool verbose) {
    auto sys_logger = spdlog::stdout_color_mt("c");
#ifdef DEBUG_LEVEL
    sys_logger->set_level(spdlog::level::debug);
#else
    sys_logger->set_level(spdlog::level::debug);
#endif
    spdlog::set_pattern("[%x %H:%M:%S] [%t] [%l] %v");
    sys_logger->info("Logger set to level {}\n>> log formatting>> [time] [thread] [logLevel] message logged", spdlog::get("c")->level());
    spdlog::get("c")->info(welcomeMsg);
}


int main(int argc, const char * const *argv) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    try {

        auto MyLogHandler = [] (google::protobuf::LogLevel level, const char* filename, int line, const std::string& message)
        {
            std::cout << "message " << message << std::endl;
        };
        google::protobuf::SetLogHandler(MyLogHandler);

        boost::asio::io_service ios;
        boost::asio::io_service::work work(ios);
        Context ctx(argc, argv);
        welcome(ctx.isVerbose());
        ctx.logContext();
        auto fysBus = std::make_shared<FysBus<fys::pb::FySMessage, 2> > (fys::pb::Type_ARRAYSIZE);
        Game::ptr game = std::make_shared<Game>(ios, ctx);

        buslistener::GamingListener gaming(game);
        GamingListener gamingListener(gaming);

        game->connectClient(ios, ctx);
        game->runGamingLoop();

        ::sleep(1);
        ios.run();
    }
    catch (std::exception &e) {
        spdlog::get("c")->error("Exception on the main, {}", e.what());
    }
    google::protobuf::ShutdownProtobufLibrary();
        return 0;
}
