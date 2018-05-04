#pragma once

typedef struct {
    volatile void *message;
    volatile void *response;
} Message;

typedef struct {
    volatile int status;
    volatile Message *message;
} MessagePort;

Message *message_get(MessagePort *port) {
    while (port->status == 0) yield();
    void *msg = port->message;
    port->status = 0;
    return msg;
}

void message_put(MessagePort *port, void *message) {
    while (port->status != 0) yield();
    port->message = message;
    port->status = 1;
}

void *message_get_response(Message *msg) {
    while (msg->response == NULL) yield();
    return msg->response;
}