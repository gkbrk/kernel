#pragma once

typedef struct {
  volatile void *message;
  volatile void *response;
} Message;

typedef struct {
  volatile int status;
  volatile Message *message;
} MessagePort;

#ifdef __cplusplus
extern "C" {
#endif

Message *message_get(MessagePort *port);
void message_put(MessagePort *port, void *message);
void *message_get_response(Message *msg);

#ifdef __cplusplus
}
#endif
