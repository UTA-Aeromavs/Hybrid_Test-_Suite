#pragma once
#include <RadioLib.h>


// Setup the radio module
bool setup_radio(Module radio);
bool send_command(Module radio);
String handle_recieved_commands(Module radio);