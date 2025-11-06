#include <cstdio>
#include <enet/enet.h>

constexpr int MAX_CLIENTS = 32;

int main() {
    if (enet_initialize() != 0) {
        printf("An error occurred while initializing ENet.\n");
        return 1;
    }

    ENetAddress address = { 0 };

    address.host = ENET_HOST_ANY; /* Bind the server to the default localhost. */
    address.port = 8080; /* Bind the server to port 7777. */


    /* create a server */
    ENetHost* server = enet_host_create(&address, MAX_CLIENTS, 2, 0, 0);

    if (server == NULL) {
        printf("An error occurred while trying to create an ENet server host.\n");
        return 1;
    }

    printf("Started a server...\n");

    ENetEvent event;

    /* Wait up to 1000 milliseconds for an event. (WARNING: blocking) */
    while (enet_host_service(server, &event, 100000) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            printf(
                "A new client connected from %x:%u.\n",
                event.peer->address.host, event.peer->address.port
            );
            /* Store any relevant client information here. */
            event.peer->data = (void*)("Client information");
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            printf(
                "A packet of length %lu containing %s was received from %s on "
                "channel %u.\n",
                event.packet->dataLength, event.packet->data,
                (char*)event.peer->data, event.channelID
            );
            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy(event.packet);
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", (char*)event.peer->data);
            /* Reset the peer's client information. */
            event.peer->data = NULL;
            break;
            // case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
            //     printf("%s disconnected due to timeout.\n", event.peer->data);
            //     /* Reset the peer's client information. */
            //     event.peer->data = NULL;
            //     break;

        case ENET_EVENT_TYPE_NONE:
            break;
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();
    return 0;
}

// #include <cstdio>
// #include <enet/enet.h>
//
// int main() {
//     if (enet_initialize() != 0) {
//         printf("An error occurred while initializing ENet.\n");
//         return 1;
//     }
//     ENetAddress address;
//     ENetHost* server = nullptr;
//
//     /* Bind the server to the default localhost.     */
//     /* A specific host address can be specified by   */
//     /* enet_address_set_host (& address, "x.x.x.x"); */
//
//     address.host = ENET_HOST_ANY;
//     /* Bind the server to port 1234. */
//     address.port = 8080;
//
//     server = enet_host_create(
//         &address /* the address to bind the server host to */,
//         32 /* allow up to 32 clients and/or outgoing connections */,
//         2 /* allow up to 2 channels to be used, 0 and 1 */,
//         0 /* assume any amount of incoming bandwidth */,
//         0 /* assume any amount of outgoing bandwidth */
//     );
//     if (server == NULL) {
//         fprintf(stderr, "An error occurred while trying to create an ENet
//         server host.\n"); exit(EXIT_FAILURE);
//     }
//
//     enet_host_destroy(server);
//
//     enet_deinitialize();
//     return 0;
// }
