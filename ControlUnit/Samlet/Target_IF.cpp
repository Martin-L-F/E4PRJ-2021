#include "Target_IF.hpp"

Target_IF::Target_IF()
{
    readConfigfile();
    if (device != "")
    {
        UARTobj_.run(device);
    }
    else
    {
        UARTobj_.run(Target_IF_PSOC_ID);
    }
    UARTobj_.addCallback(bind(&Target_IF::UARTDispatcher, this, placeholders::_1, placeholders::_2));
}

Target_IF::~Target_IF()
{
}

void Target_IF::startDetection(int numberOfPlayers)
{
    uint8_t msg[3];

    msg[0] = 0xD3;
    msg[1] = numberOfPlayers;
    msg[2] = 0xEF;

    UARTobj_.writeMsg(msg, 3);
}

void Target_IF::stopDetection()
{
    uint8_t msg[2];

    msg[0] = 0xD7;
    msg[1] = 0xEF;

    UARTobj_.writeMsg(msg, 2);
}

void Target_IF::UARTDispatcher(unsigned id, Message *msg)
{
    if (debug_UART)
        printf("Target Dispatcher id: %x\n", id);

    switch (id)
    {
    case (unsigned)0xD5:
        updateScore(msg);
        break;
    default:
        if (debug_UART)
            cout << "Target Dispatcher: unknown id" << endl;
        ;
    }
}

int Target_IF::updateScore(Message *user) //Handler in TargetIF_Dispatcher
{

    Rx_data *rxObj = static_cast<Rx_data *>(user);

    callbackControllershotDetected_(rxObj->getMsgData());

    return 0;
}

void Target_IF::addCallback(function<void(int)> cb)
{
    callbackControllershotDetected_ = cb;
}

void Target_IF::readConfigfile()
{
    try
    {
        std::ifstream configStream("config.json");
        json config;
        if (configStream.is_open())
        {
            std::cout << "[Target_IF][INFO] Configurationfile found." << std::endl;
            configStream >> config;
            configStream.close();

            if (config.is_object())
            {
                // Get device
                if (config["Target_IF"]["device"].is_string())
                {
                    device = config["Target_IF"]["device"];
                    std::cout << "[Target_IF][INFO] Device changed to '" << device << "'." << std::endl;
                }
            }
        }
    }
    catch (const std::exception &ex)
    {
        std::cout << "[Target_IF][WARN] An error occurred while reading configurationfile." << std::endl;
        std::cout << "[Target_IF][WARN] " << ex.what() << std::endl;
    }
}