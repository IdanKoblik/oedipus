#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <cstdint>
#include <google/protobuf/message.h>
#include "editor.h"

bool sendProto(int fd, const google::protobuf::Message& msg);

bool sendFile(int fd, const std::string& path, uint64_t clientId);

void startServer(editor::TextEditor* editor, const NetworkBinding& bind);
void closeServer(editor::TextEditor* editor);

#endif // SERVER_H