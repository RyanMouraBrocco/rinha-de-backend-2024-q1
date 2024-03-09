// C++ libraries
#include <iostream>
#include "Socket/socket.hpp"
#include "Connections/connection.hpp"
#include "Dto/Customer/customer_dto.pb.h"
#include "Dto/Transaction/transaction_dto.pb.h"
#include "Data/customer_data.hpp"

int main()
{
    Socket socket = Socket(5001);
    socket.Listen();
    if (socket.GetStatus() != SocketStatus::Opened)
    {
        return -1;
    }

    Connection connection = Connection(socket);
    connection.AcceptForNewAccess();

    char buf[4096];
    while (true)
    {
        std::string text = connection.ReceiveBytes(4096);

        if (connection.GetStatus() != ConnectionStatus::Connected)
            break;

        std::cout << "Receive data: " << text << std::endl;

        connection.SendBytes(text.data(), sizeof(text) + 1);
    }

    return 0;
}