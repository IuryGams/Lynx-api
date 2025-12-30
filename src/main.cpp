/* Controllers */
#include "controllers/customer_controller.h"
#include "controllers/order_controller.h"
#include "controllers/payment_controller.h"
#include "controllers/product_controller.h"

// Repositories
#include "repository/customer_repository.h"
#include "repository/order_repository.h"
#include "repository/payment_repository.h"
#include "repository/product_repository.h"

// Services
#include "services/customer_services.h"
#include "services/order_services.h"
#include "services/payment_services.h"
#include "services/product_services.h"

#include "server.h"

int main()
{
    using namespace lynx;

    try
    {
        // ======================
        // Server configuration
        // ======================
        server::ServerConfig config;
        config.port = 8000;
        config.threads = 4;
        config.log_level = "debug";
        config.cors = true;
        config.cors_origin = "*";

        auto server = std::make_unique<server::Server>(config);

        // ======================
        // Repositories
        // ======================
        auto customer_repository = std::make_shared<repository::CustomerRepository>();
        auto product_repository = std::make_shared<repository::ProductRepository>();
        auto order_repository = std::make_shared<repository::OrderRepository>();
        auto payment_repository = std::make_shared<repository::PaymentRepository>();

        // ======================
        // Services
        // ======================
        auto customer_service = std::make_shared<services::CustomerServices>(customer_repository);
        auto product_service = std::make_shared<services::ProductServices>(product_repository);
        auto order_service = std::make_shared<services::OrderServices>(order_repository, product_repository, customer_repository);
        auto payment_service = std::make_shared<services::PaymentServices>(payment_repository, order_service);

        // ======================
        // Controllers (Handlers)
        // ======================
        server->add_handler(std::make_shared<controller::CustomerController>(customer_service));

        server->add_handler(std::make_shared<controller::ProductController>(product_service));

        server->add_handler(std::make_shared<controller::OrderController>(order_service));

        server->add_handler(std::make_shared<controller::PaymentController>(payment_service));

        // ======================
        // Start server
        // ======================
        server->start();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
}