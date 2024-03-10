#include "../nlohmann/json.hpp"
#include <string>
#include <queue>
#include "../Dto/Customer/customer_dto.pb.h"
#include "../Dto/Transaction/transaction_dto.pb.h"

inline std::string TimestampToString(const google::protobuf::Timestamp &timestamp)
{
    // Convert seconds and nanos to a time_point
    std::chrono::seconds seconds(timestamp.seconds());
    std::chrono::milliseconds milliseconds(timestamp.nanos() / 1000000); // Convert nanoseconds to milliseconds
    std::chrono::system_clock::time_point time_point = std::chrono::time_point<std::chrono::system_clock>(seconds + milliseconds);

    // Convert time_point to std::tm for formatting
    time_t time = std::chrono::system_clock::to_time_t(time_point);
    std::tm tm_time = *std::localtime(&time);

    // Format the datetime string
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%dT%H:%M:%S.");                    // Format with milliseconds
    oss << std::setfill('0') << std::setw(3) << timestamp.nanos() % 1000000; // Append milliseconds
    oss << "Z";                                                              // Append 'Z' for UTC timezone
    return oss.str();
}

struct GetCustomerWithJoinInTransactionsResult
{
    CustomerDto customer;
    std::queue<TransactionDto> transactions;

    std::string SerializeJson()
    {
        nlohmann::json resultJson;
        nlohmann::json customerJson;
        nlohmann::json transactionsListJson = nlohmann::json::array();

        customerJson["limite"] = customer.limit();
        customerJson["total"] = customer.balance();

        while (!transactions.empty())
        {
            nlohmann::json transactionJson;
            TransactionDto &transaction = transactions.front();
            transactionJson["valor"] = transaction.value();
            if (transaction.iscredit())
                transactionJson["tipo"] = "c";
            else
                transactionJson["tipo"] = "d";

            transactionJson["descricao"] = transaction.description();
            transactionJson["realizada_em"] = TimestampToString(transaction.createdate());

            transactionsListJson.push_back(transactionJson);

            transactions.pop();
        }

        resultJson["saldo"] = customerJson;
        resultJson["ultimas_transacoes"] = transactionsListJson;
        return resultJson.dump();
    }
};

struct CreditTransactionResult
{
    CustomerDto customer;

    std::string SerializeJson()
    {
        nlohmann::json customerJson;
        customerJson["limite"] = customer.limit();
        customerJson["saldo"] = customer.balance();
        return customerJson.dump();
    }
};

struct DebtTransactionResult
{
    CustomerDto customer;
    bool success;

    std::string SerializeJson()
    {
        nlohmann::json customerJson;
        customerJson["limite"] = customer.limit();
        customerJson["saldo"] = customer.balance();
        return customerJson.dump();
    }
};