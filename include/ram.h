
#pragma once

#include <cstring>
#include <vector>
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include "common.h"

struct Ram : sc_core::sc_module
{
  tlm_utils::simple_target_socket<Ram> socket;
  std::vector<unsigned char> data;

  SC_CTOR(Ram) : socket("socket"), data(map::RAM_SIZE, 0)
  {
    socket.register_b_transport(this, &Ram::b_transport);
  }

  void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
  {
    const auto cmd  = trans.get_command();
    const auto addr = trans.get_address();
    auto*      ptr  = trans.get_data_ptr();
    const auto len  = trans.get_data_length();

    // verificar que la direccion no se sale del rango de la RAM
    if ((addr + len) > data.size())
    {
      trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
      return;
    }

    if (cmd == tlm::TLM_WRITE_COMMAND)
      std::memcpy(&data[addr], ptr, len);
    else
      std::memcpy(ptr, &data[addr], len);

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
  }
};
