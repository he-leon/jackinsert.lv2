// GNU LGPL v3 
// Copyright (C) 2024  He-Leon
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <jack/jack.h>
#include <lv2.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLUGIN_URI "https://github.com/he-leon/jackinsert"


volatile int done        = 0;
volatile int slconnected = 0;

typedef enum {
  INPUT_LEFT   = 0,
  INPUT_RIGHT  = 1,
  OUTPUT_LEFT  = 2,
  OUTPUT_RIGHT = 3,
  SEND_LEFT    = 4,
  SEND_RIGHT   = 5,
  RETURN_LEFT  = 6,
  RETURN_RIGHT = 7
} PortIndex;

typedef struct {
  float*       input_left;
  float*       input_right;
  float*       output_left;
  float*       output_right;
  float*       send_left;
  float*       send_right;
  float*       return_left;
  float*       return_right;
} JackInsert;

static void connect_jack_ports(int disconnect) {
  if (disconnect) {
    fprintf(stderr, "Disconnecting JACK ports\n");
  } else {
    fprintf(stderr, "Connecting JACK ports\n");
  }
  const char*    plugin_name = "JACKInsert";
  jack_client_t* client;
  jack_options_t options = JackNoStartServer;
  jack_status_t  status;
  char*          server_name = NULL;
  if ((client = jack_client_open("jack_connect", options, &status, server_name)) == 0) {
    fprintf(stderr, "jack server not running?\n");
  }

  if (!client) {
    fprintf(stderr, "Failed to create JACK client for connections\n");
    return;
  }
  jack_activate(client);

  const char** ports;
  const char** our_ports;
  // Connect/disconnect to Sooperlooper input ports
  ports     = jack_get_ports(client, "sooperlooper:common_in_", NULL, JackPortIsInput);
  our_ports = jack_get_ports(client, "JACKInsert:send_", NULL, JackPortIsOutput);
  int ret;
  if (ports && our_ports) {
    if (disconnect) {
      fprintf(stderr, "Disconnecting from Sooperlooper input ports\n");
    } else {
      fprintf(stderr, "Connecting to Sooperlooper input ports\n");
    }
    fprintf(stderr, "  %s -> %s\n", our_ports[0], ports[0]);
    if (disconnect) {
      ret = jack_disconnect(client, our_ports[0], ports[0]);
    } else {
      ret = jack_connect(client, our_ports[0], ports[0]);
    }
    if (ret != 0) {
      if (disconnect) {
        fprintf(stderr, "Failed to disconnect %s -> %s with return code %d\n", our_ports[0],
                ports[0], ret);
      } else {
        fprintf(stderr, "Failed to connect %s -> %s with return code %d\n", our_ports[0], ports[0],
                ret);
      }
    }
    fprintf(stderr, "  %s -> %s\n", our_ports[1], ports[1]);
    if (disconnect) {
      ret = jack_disconnect(client, our_ports[1], ports[1]);
    } else {
      ret = jack_connect(client, our_ports[1], ports[1]);
    }

    if (ret != 0) {
      if (disconnect) {
        fprintf(stderr, "Failed to disconnect %s -> %s with return code %d\n", our_ports[1],
                ports[1], ret);
      } else {
        fprintf(stderr, "Failed to connect %s -> %s with return code %d\n", our_ports[1], ports[1],
                ret);
      }
    }
  } else {
    if (disconnect) {
      fprintf(stderr, "Failed to disconnect from Sooperlooper input ports\n");
    } else {
      fprintf(stderr, "Failed to connect to Sooperlooper input ports\n");
    }
  }

  // Connect/Disconnect to Sooperlooper output ports
  ports     = jack_get_ports(client, "sooperlooper:common_out_", NULL, JackPortIsOutput);
  our_ports = jack_get_ports(client, "JACKInsert:return_", NULL, JackPortIsInput);
  if (ports && our_ports) {
    if (disconnect) {
      fprintf(stderr, "Disconnecting from Sooperlooper output ports\n");
    } else {
      fprintf(stderr, "Connecting to Sooperlooper output ports\n");
    }

    fprintf(stderr, "  %s -> %s\n", ports[1], our_ports[1]);
    if (disconnect) {
      ret = jack_disconnect(client, ports[1], our_ports[1]);
    } else {
      ret = jack_connect(client, ports[1], our_ports[1]);
    }
    if (ret != 0) {
      if (disconnect) {
        fprintf(stderr, "Failed to disconnect %s -> %s with return code %d\n", ports[1],
                our_ports[1], ret);
      } else {
        fprintf(stderr, "Failed to connect %s -> %s with return code %d\n", ports[1], our_ports[1],
                ret);
      }
    }
    fprintf(stderr, "  %s -> %s\n", ports[0], our_ports[0]);
    if (disconnect) {
      ret = jack_disconnect(client, ports[0], our_ports[0]);
    } else {
      ret = jack_connect(client, ports[0], our_ports[0]);
    }
    if (ret != 0) {
      if (disconnect) {
        fprintf(stderr, "Failed to disconnect %s -> %s with return code %d\n", ports[0],
                our_ports[0], ret);
      } else {
        fprintf(stderr, "Failed to connect %s -> %s with return code %d\n", ports[0], our_ports[0],
                ret);
      }
    } else {
      slconnected = 1;
    }
  } else {
    if (disconnect) {
      fprintf(stderr, "Failed to disconnect from Sooperlooper output ports\n");
    } else {
      fprintf(stderr, "Failed to connect to Sooperlooper output ports\n");
    }
  }

  jack_client_close(client);
}

static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double rate,
                              const char* bundle_path, const LV2_Feature* const* features) {
  JackInsert* self = (JackInsert*)calloc(1, sizeof(JackInsert));
  if (!self) return NULL;

  return (LV2_Handle)self;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data) {
  JackInsert* self = (JackInsert*)instance;

  switch ((PortIndex)port) {
    case INPUT_LEFT:
      self->input_left = (float*)data;
      break;
    case INPUT_RIGHT:
      self->input_right = (float*)data;
      break;
    case OUTPUT_LEFT:
      self->output_left = (float*)data;
      break;
    case OUTPUT_RIGHT:
      self->output_right = (float*)data;
      break;
    case SEND_LEFT:
      self->send_left = (float*)data;
      break;
    case SEND_RIGHT:
      self->send_right = (float*)data;
      break;
    case RETURN_LEFT:
      self->return_left = (float*)data;
      break;
    case RETURN_RIGHT:
      self->return_right = (float*)data;
      break;
  }
}

static void activate(LV2_Handle instance) {
  // Nothing to do here
}

static void run(LV2_Handle instance, uint32_t n_samples) {
  JackInsert* self = (JackInsert*)instance;

  // Need to do this here because we can't do it in activate as the JACK client
  // has not been activated at that point. There is probably a better way to do this.
  // This will lead to some XRUNs :/
  if (!slconnected) {
    connect_jack_ports(0);
  }

  // Copy input to SEND ports
  memcpy(self->send_left, self->input_left, sizeof(float) * n_samples);
  memcpy(self->send_right, self->input_right, sizeof(float) * n_samples);

  // Copy RETURN port to output
  memcpy(self->output_left, self->return_left, sizeof(float) * n_samples);
  memcpy(self->output_right, self->return_right, sizeof(float) * n_samples);
}

static void deactivate(LV2_Handle instance) {}

static void cleanup(LV2_Handle instance) { free(instance); }

static const void* extension_data(const char* uri) { return NULL; }

static const LV2_Descriptor descriptor = {PLUGIN_URI, instantiate, connect_port, activate,
                                          run,        deactivate,  cleanup,      extension_data};

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index) { return (index == 0) ? &descriptor : NULL; }
