#pragma once

typedef struct {
  void *message;
  void *response;
} Message;

typedef struct {
  int status;
  Message *message;
} MessagePort;

#ifdef __cplusplus
extern "C" {
#endif

Message *message_get(MessagePort *port);
void message_put(volatile MessagePort *port, void *message);
void *message_get_response(Message *msg);

#ifdef __cplusplus
}
#endif
