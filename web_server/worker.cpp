//
// Created by Nikita on 20.12.2021.
//
#include <boost/program_options.hpp>
#include <iostream>
#include <csignal>
#include <cppkafka/consumer.h>
#include <cppkafka/configuration.h>
#include "config/config.h"

#include "models/person.h"

namespace po = boost::program_options;

bool running = true;

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    try
    {
        po::options_description desc{"Options"};
        desc.add_options()
            ("help,", "This screen")
            ("queue_host,", po::value<std::string>()->required(), "queue host")
            ("queue_topic,", po::value<std::string>()->required(), "queue_topic")
            ("queue_group_id,", po::value<std::string>()->required(), "queue group id")
            ("db_host,", po::value<std::string>()->required(), "database host")
            ("db_port,", po::value<std::string>()->required(), "database port")
            ("db_login,", po::value<std::string>()->required(), "database login")
            ("db_password,", po::value<std::string>()->required(), "database password")
            ("db_name,", po::value<std::string>()->required(), "database name");

        po::variables_map vm;
        po::store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
            std::cout << desc << '\n';

        if (vm.count("queue_host")) Config::get().queue_host() = vm["queue_host"].as<std::string>();
        if (vm.count("queue_topic")) Config::get().queue_topic() = vm["queue_topic"].as<std::string>();
        if (vm.count("queue_group_id")) Config::get().queue_group_id() = vm["queue_group_id"].as<std::string>();
        if (vm.count("db_host")) Config::get().db_host() = vm["db_host"].as<std::string>();
        if (vm.count("db_port")) Config::get().db_port() = vm["db_port"].as<std::string>();
        if (vm.count("db_login")) Config::get().db_login() = vm["db_login"].as<std::string>();
        if (vm.count("db_password")) Config::get().db_password() = vm["db_password"].as<std::string>();
        if (vm.count("db_name")) Config::get().db_name() = vm["db_name"].as<std::string>();

        // Stop processing on SIGINT
        signal(SIGINT, [](int) { running = false; });

        // Construct the configuration
        cppkafka::Configuration config = {
                {"metadata.broker.list", Config::get().get_queue_host()},
                {"group.id", Config::get().get_queue_group_id()},
                {"enable.auto.commit", false}  // Disable auto commit
        };

        // Create the consumer
        cppkafka::Consumer consumer(config);

        // Print the assigned partitions on assignment
        consumer.set_assignment_callback([](const cppkafka::TopicPartitionList &partitions) {
            std::cout << "Got assigned: " << partitions << std::endl;
        });

        // Print the revoked partitions on revocation
        consumer.set_revocation_callback([](const cppkafka::TopicPartitionList &partitions) {
            std::cout << "Got revoked: " << partitions << std::endl;
        });

        // Subscribe to the topic
        consumer.subscribe({Config::get().get_queue_topic()});
        std::cout << "Consuming messages from topic " << Config::get().get_queue_topic() << std::endl;

        // Now read lines and write them into kafka
        while (running) {
            // Try to consume a message
            cppkafka::Message msg = consumer.poll();
            if (msg) {
                if (msg.get_error()) {  // If we managed to get a message
                    if (!msg.is_eof()) std::cout << "[+] Received error notification: " << msg.get_error() << std::endl;  // Ignore EOF notifications from rdkafka
                } else {
                    if (msg.get_key()) std::cout << msg.get_key() << " -> ";  // Print the key (if any)

                    // Print the payload
                    std::string payload = msg.get_payload();
                    std::cout << msg.get_payload() << std::endl;
                    database::Person person = database::Person::fromJSON(payload);
                    person.db_save();

                    consumer.commit(msg);  // Now commit the message
                }
            }
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 1;
}
