#include <stddef.h>

#include "../scheduler.h"
#include "messaging.h"

Message *message_get(MessagePort *port) {
  while (port->status == 0)
    yield();
  volatile void *msg = port->message;
  port->status = 0;
  return msg;
}

void message_put(volatile MessagePort *port, void *message) {
  while (port->status != 0)
    yield();
  port->message = message;
  port->status = 1;
}

void *message_get_response(Message *msg) {
  while (msg->response == NULL)
    yield();
  return msg->response;
}
